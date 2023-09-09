# agent.py
import heapq
import random
from modules.grids import *
from modules.products import * 

ENTRANCE = (299, 180)
MOVES = [(-1, 0), (-1, 1), (0, 1), (1, 1), (1, 0), (1, -1), (0, -1), (-1, -1)]

class Agent: 
    def __init__(self, idx, loc, target, target_type, target_loc): 
        self.idx = idx
        self.loc = loc
        self.target = target
        self.target_type = target_type
        self.purchase = False
        self.target_loc = target_loc
            
    def a_star(self, grid, start, end):
        open_list = []
        heapq.heappush(open_list, (0 + euclideanDist(start, end), 0, start))
        came_from = {}
        g_score = {start: 0}
        f_score = {start: euclideanDist(start, end)}

        while open_list:
            _, current_g, current = heapq.heappop(open_list)

            if current == end:
                return self.reconstruct_path(came_from, current)

            for move in MOVES:
                neighbor = addVec(current, move)
                tentative_g_score = current_g + 1  

                # Check if the neighbor is valid
                x, y = neighbor
                if not grid.inBounds(y, x) or grid[y][x].type not in ['empty', 'enter', 'product']:
                    continue

                # Check if this path to neighbor is better than previous
                if neighbor not in g_score or tentative_g_score < g_score[neighbor]:
                    came_from[neighbor] = current
                    g_score[neighbor] = tentative_g_score
                    f_score[neighbor] = tentative_g_score + euclideanDist(neighbor, end)
                    heapq.heappush(open_list, (f_score[neighbor], tentative_g_score, neighbor))

        return None  # No path found

    def reconstruct_path(self, came_from, current):
        path = [current]
        while current in came_from:
            current = came_from[current]
            path.append(current)
        path.reverse()
        return path

    def move(self, grid, quantity_dict):
        path = self.a_star(grid, self.loc, self.target_loc)
        if not path or len(path) <= 1:
            return
        x_new, y_new = path[1]
        x, y = self.loc
        
        if grid[y][x].type == 'agent_product': 
            if not self.purchase and (x, y) == self.target_loc: 
                self.purchaseItem(grid, quantity_dict)
            else: 
                grid[y][x].type = 'product'
        else: 
            grid[y][x].type = 'empty'
        if grid[y_new][x_new].type == 'product': 
            grid[y_new][x_new].type = 'agent_product'
        else: 
            if self.purchase: 
                grid[y_new][x_new].type = 'agent_return'
            else:
                grid[y_new][x_new].type = 'agent'
        self.loc = x_new, y_new
    
    def purchaseItem(self, grid, quantity_dict):
        x, y = self.loc
        if quantity_dict[self.target] >= 1:
            quantity_dict[self.target] -= 1
            if quantity_dict[self.target] <= 0: 
                grid[y][x].type = 'obstacle'
            else: 
                grid[y][x].type = 'product'
        self.target_loc = ENTRANCE
        self.purchase = True
        
def initSingleAgent(idx, loc, names, popularity, quantity_dict, category_dict, product_locations): 
    target = sampleRandomProduct(names, popularity, quantity_dict)
    agent = Agent(idx, loc, target, category_dict[target], product_locations[target])
    return agent
    
def initAgents(grid, names, popularity, quantity_dict, category_dict, product_locations, num = 2500):
    agents = []
    entrance = [(row_idx, col_idx) for row_idx, row in enumerate(grid.grid)
                   for col_idx, tile in enumerate(row) if tile.type == 'enter']
    random.shuffle(entrance)
    for idx, (row_idx, col_idx) in enumerate(entrance):
        if idx == num: 
            break
        grid[row_idx][col_idx].type = 'agent'
        agent_init_loc = grid[row_idx][col_idx].loc
        agents.append(initSingleAgent(idx, agent_init_loc, names, popularity, quantity_dict, category_dict, product_locations))
    return agents

def moveAgents(grid, agents, quantity_dict): 
    for agent in agents: 
        agent.move(grid, quantity_dict)
