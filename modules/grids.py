import math
import io
from PIL import Image
import matplotlib.pyplot as plt
from modules.importLayout import * 

PIXEL_THRESHOLD = 250
color_map = {
    'empty': [1, 1, 1],
    'enter': [1, 1, 1],
    'obstacle': [0, 0, 0],
    'wall': [0.5, 0.5, 0.5],
    'agent': [1, 0, 0],
    'product': [.08, .96, .93],
    'agent_product': [1, 0, 0],
    'agent_return': [0, 0, 1]
}

class Tile: 
    def __init__(self, x, y, type, product = None): 
        self.loc = (x, y)
        self.type = type
        self.product = product
    
    def isObstacle(self): 
        return self.type != 'empty' and self.type != 'enter'

def getType(img_resized, adjusted_x, adjusted_y): 
    if adjusted_y >= DIM_UNITS[1] - 1 and DIM_UNITS[0] * 0.6 <= adjusted_x < 0.9 * img_resized.size[0]:
        return 'enter'
    if 0 > adjusted_x or adjusted_x >= img_resized.size[0] - 1: 
        return 'wall'
    elif 0 > adjusted_y or adjusted_y >= img_resized.size[1] - 1: 
        return 'wall'
    elif img_resized.getpixel((adjusted_x, adjusted_y)) < PIXEL_THRESHOLD: 
        return 'obstacle'
    return 'empty'

def addVec(vec1, vec2): 
    x1, y1 = vec1
    x2, y2 = vec2
    return (x1 + x2, y1 + y2)

def euclideanDist(loc1, loc2): 
    x1, y1 = loc1
    x2, y2 = loc2
    dist = math.sqrt((x1 - x2)**2 + (y1 - y2)**2)
    return dist

class Grid: 
    def __init__(self, img_resized):
        self.grid = self._generate_grid(img_resized)
        self.shape = (2 * img_resized.size[0], 2 * img_resized.size[1])
    
    def _generate_grid(self, img_resized): 
        grid = []
        for y in range(2 * DIM_UNITS[1]): 
            row = []
            for x in range(2 * DIM_UNITS[0]): 
                type = getType(img_resized, x - DIM_UNITS[0] / 2 + 1, y - DIM_UNITS[1] / 2 + 1)
                row.append(Tile(x, y, type))
            grid.append(row)
        return grid
    
    def __getitem__(self, index):
        return self.grid[index]
    
    def plot(self):

        grid_array = np.array([[color_map[tile.type] for tile in row] for row in self.grid])
        fig, ax = plt.subplots(figsize=(10, 10))
        ax.imshow(grid_array)
        plt.show()
        
    def plotImage(self): 
        grid_array = np.array([[color_map[tile.type] for tile in row] for row in self.grid])
        fig, ax = plt.subplots(figsize=(10, 10))
        ax.imshow(grid_array)

        buf = io.BytesIO()
        plt.savefig(buf, format="png")
        plt.close(fig)  # Close the figure to suppress output
        buf.seek(0)
        
        img = Image.open(buf)
        return img
    
    def inBounds(self, x_new, y_new): 
        x_max, y_max = self.shape
        return 0 <= x_new < x_max and 0 <= y_new < y_max