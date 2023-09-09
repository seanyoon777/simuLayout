#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <algorithm>
#include <string>
#include <random>  
#include "headers/grids.hpp"
#include "headers/products.hpp"

const std::pair<int, int> ENTRANCE = {299, 180};
const std::vector<std::pair<int, int>> MOVES = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

class Agent {
public:
    Agent(int idx, std::pair<int, int> loc, std::string target, std::string target_type, std::pair<int, int> target_loc)
        : idx(idx), loc(loc), target(target), target_type(target_type), target_loc(target_loc), purchase(false) {}

    std::vector<std::pair<int, int>> reconstruct_path(const std::unordered_map<std::pair<int, int>, std::pair<int, int>>& came_from, std::pair<int, int> current);
    std::vector<std::pair<int, int>> a_star(Grid& grid, std::pair<int, int> start, std::pair<int, int> end);
    void move(Grid& grid, std::unordered_map<std::string, int>& quantity_dict);
    void purchaseItem(Grid& grid, std::unordered_map<std::string, int>& quantity_dict);

private:
    int idx;
    std::pair<int, int> loc;
    std::string target;
    std::string target_type;
    bool purchase;
    std::pair<int, int> target_loc;
    std::vector<std::vector<double>> g_score = std::vector<std::vector<double>>(GRID_WIDTH, std::vector<double>(GRID_HEIGHT, std::numeric_limits<double>::max()));
    std::vector<std::vector<double>> f_score = std::vector<std::vector<double>>(GRID_WIDTH, std::vector<double>(GRID_HEIGHT, std::numeric_limits<double>::max()));


    std::priority_queue<std::tuple<double, double, std::pair<int, int>>> open_list; 
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, std::hash<std::pair<int, int>>> came_from;
};

std::vector<std::pair<int, int>> Agent::reconstruct_path(const std::unordered_map<std::pair<int, int>, std::pair<int, int>>& came_from, std::pair<int, int> current) {
    std::vector<std::pair<int, int>> path;
    while (came_from.find(current) != came_from.end()) {
        path.push_back(current);
        current = came_from.at(current);
    }
    path.push_back(current);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<std::pair<int, int>> Agent::a_star(Grid& grid, std::pair<int, int> start, std::pair<int, int> end) {
    // Reset the data structures
    for (auto& row : g_score) std::fill(row.begin(), row.end(), std::numeric_limits<double>::max());
    for (auto& row : f_score) std::fill(row.begin(), row.end(), std::numeric_limits<double>::max());
    came_from.clear();
    while (!open_list.empty()) open_list.pop();

    open_list.push({0 + euclideanDist(start, end), 0, start});
    g_score[start.first][start.second] = 0;
    f_score[start.first][start.second] = euclideanDist(start, end);

    while (!open_list.empty()) {
        double current_f, current_g;
        std::pair<int, int> current;
        std::tie(current_f, current_g, current) = open_list.top();
        open_list.pop();

        if (current == end) {
            return reconstruct_path(came_from, current);
        }

        for (const auto& move : MOVES) {
            std::pair<int, int> neighbor = {current.first + move.first, current.second + move.second};
            double tentative_g_score = current_g + 1.0;

            if (!grid.inBounds(neighbor.second, neighbor.first) ||
                !(grid.typeAt(neighbor.second, neighbor.first) == "empty" || grid.typeAt(neighbor.second, neighbor.first) == "enter" || grid.typeAt(neighbor.second, neighbor.first) == "product")) {
                continue;
            }

            if (tentative_g_score < g_score[neighbor.first][neighbor.second]) {
                came_from[neighbor] = current;
                g_score[neighbor.first][neighbor.second] = tentative_g_score;
                f_score[neighbor.first][neighbor.second] = tentative_g_score + euclideanDist(neighbor, end);
                open_list.push({f_score[neighbor.first][neighbor.second], tentative_g_score, neighbor});
            }
        }
    }
    return {}; 
}

void Agent::move(Grid& grid, std::unordered_map<std::string, int>& quantity_dict) {
    auto path = a_star(grid, loc, target_loc);
    if (path.size() <= 1) return;

    std::pair<int, int> new_loc = path[1];

    if (grid.typeAt(loc.second, loc.first) == "agent_product") {
        if (!purchase && loc == target_loc) {
            purchaseItem(grid, quantity_dict);
        } else {
            grid.setTypeAt(loc.second, loc.first, "product");
        }
    } else {
        grid.setTypeAt(loc.second, loc.first, "empty");
    }

    if (grid.typeAt(new_loc.second, new_loc.first) == "product") {
        grid.setTypeAt(new_loc.second, new_loc.first, "agent_product");
    } else {
        grid.setTypeAt(new_loc.second, new_loc.first, purchase ? "agent_return" : "agent");
    }

    loc = new_loc;
}

void Agent::purchaseItem(Grid& grid, std::unordered_map<std::string, int>& quantity_dict) {
    if (quantity_dict[target] > 0) {
        quantity_dict[target]--;
        if (quantity_dict[target] <= 0) {
            grid.setTypeAt(loc.second, loc.first, "obstacle");
        } else {
            grid.setTypeAt(loc.second, loc.first, "product");
        }
        target_loc = ENTRANCE;
        purchase = true;
    }
}

Agent initSingleAgent(int idx, std::pair<int, int> loc, 
                     const std::vector<std::string>& names, 
                     const std::vector<double>& popularity, 
                     std::unordered_map<std::string, int>& quantity_dict,
                     std::unordered_map<std::string, std::string>& category_dict, 
                     std::unordered_map<std::string, std::pair<int, int>>& product_locations) {
    
    std::string target = sampleRandomProduct(names, popularity, quantity_dict);
    return Agent(idx, loc, target, category_dict[target], product_locations[target]);
}

std::vector<Agent> initAgents(Grid& grid, 
                             const std::vector<std::string>& names, 
                             const std::vector<double>& popularity, 
                             std::unordered_map<std::string, int>& quantity_dict, 
                             std::unordered_map<std::string, std::string>& category_dict,
                             std::unordered_map<std::string, std::pair<int, int>>& product_locations, 
                             int num = 2500) {
    
    std::vector<std::pair<int, int>> entrance;
    for(int row_idx = 0; row_idx < grid.size(); ++row_idx) {
        for(int col_idx = 0; col_idx < grid[row_idx].size(); ++col_idx) {
            if(grid[row_idx][col_idx].type == "enter") {
                entrance.push_back({row_idx, col_idx});
            }
        }
    }

    std::shuffle(entrance.begin(), entrance.end(), std::default_random_engine());

    std::vector<Agent> agents;
    for(int idx = 0; idx < num && idx < entrance.size(); ++idx) {
        grid[entrance[idx].first][entrance[idx].second].type = "agent";
        std::pair<int, int> agent_init_loc = grid[entrance[idx].first][entrance[idx].second].loc;
        agents.push_back(initSingleAgent(idx, agent_init_loc, names, popularity, quantity_dict, category_dict, product_locations));
    }

    return agents;
}

void moveAgents(Grid& grid, 
                std::vector<Agent>& agents, 
                std::unordered_map<std::string, int>& quantity_dict) {
    for(auto& agent : agents) {
        agent.move(grid, quantity_dict);
    }
}
