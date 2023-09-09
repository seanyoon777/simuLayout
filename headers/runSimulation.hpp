#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <random>
#include <filesystem>
#include "headers/importLayout.hpp"
#include "headers/grids.hpp"  
#include "headers/products.hpp"
#include "headers/agents.hpp"

const int TIME = 1000;

std::string generateID(int n = 7) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(pow(10, n - 1), pow(10, n) - 1);
    return std::to_string(dist(gen));
}

void runSimulation(const std::string& layout_filepath, const std::string& df_filepath, const std::string& destination, int agent_num = 2500, int time = TIME) {
    Image img(layout_filepath); 
    Image img_resized = img.resize(DIM_UNITS); 
    Grid grid(img_resized);

    DataFrame df;
    df.read_from_csv(df_filepath);

    auto [names, popularity] = getPopularityList(df);
    auto quantity_dict = getQuantityDict(df);
    auto category_dict = getCategoryDict(df);
    auto possible_coords = getPossibleProductLocs(grid);
    auto [product_locations, new_grid] = initProductLocations(df, possible_coords, grid);

    std::vector<Agent> agents = initAgents(grid, names, popularity, quantity_dict, category_dict, product_locations, agent_num);
    std::vector<Image> frames;

    for (int t = 0; t < time; t++) {
        Image frame = grid.plotImage();
        frames.push_back(frame);
        moveAgents(grid, agents, quantity_dict, product_locations);
    }

    std::string output_pathname = std::filesystem::path(destination) / ("simulation_" + generateID() + ".gif");
    frames[0].save(output_pathname, frames); 
}
