#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "headers/grids.hpp"

const std::vector<double> FACTOR_WEIGHTS = {1, 0.5, 0.5};
const double MAX_POPULARITY = 2.0;
const int WINDOW = 2;
const int WINDOW_SIZE = (WINDOW * 2) * (WINDOW * 2) - 1;

class Tile {
public:
    int x, y;
    std::string type;
    std::string product;

    Tile() : x(0), y(0), type("empty"), product("") {}
};

class DataFrame {
public:
    std::vector<std::string> Item;
    std::vector<double> Rating;
    std::vector<double> RegularPrice;
    std::vector<double> DiscountPrice;
    std::vector<double> Quantity;
    std::vector<std::string> ProductCategory;

    void read_from_csv(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        std::getline(file, line);  // Skip header
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string field;

            std::getline(ss, field, ',');
            Item.push_back(field);
            std::getline(ss, field, ',');
            Rating.push_back(std::stod(field));
            std::getline(ss, field, ',');
            RegularPrice.push_back(std::stod(field));
            std::getline(ss, field, ',');
            DiscountPrice.push_back(std::stod(field));
            std::getline(ss, field, ',');
            Quantity.push_back(std::stod(field));
            std::getline(ss, field, ',');
            ProductCategory.push_back(field);
        }
    }
};

std::vector<double> getItemPopularity(const std::vector<double>& col1, 
                                      const std::vector<double>& col2,
                                      const std::vector<double>& col3) {
    std::vector<double> popularity;
    for(size_t i = 0; i < col1.size(); i++) {
        popularity.push_back(col1[i] * FACTOR_WEIGHTS[0] + col2[i] * FACTOR_WEIGHTS[1] + col3[i] * FACTOR_WEIGHTS[2]);
    }
    return popularity;
}

std::pair<std::vector<std::string>, std::vector<double>> getPopularityList(const DataFrame& df) {
    std::vector<double> norm_rating(df.Rating.size());
    std::vector<double> norm_discount(df.DiscountPrice.size());
    std::vector<double> norm_quantity(df.Quantity.size());
    
    // Normalize ratings, quantities, and compute discounts
    double min_quantity = *std::min_element(df.Quantity.begin(), df.Quantity.end());
    double max_quantity = *std::max_element(df.Quantity.begin(), df.Quantity.end());
    for(size_t i = 0; i < df.Rating.size(); i++) {
        norm_rating[i] = df.Rating[i] / 5.0;
        norm_discount[i] = (df.RegularPrice[i] - df.DiscountPrice[i]) / df.RegularPrice[i];
        norm_quantity[i] = (df.Quantity[i] - min_quantity) / (max_quantity - min_quantity);
    }
    auto popularity = getItemPopularity(norm_discount, norm_quantity, norm_rating);
    for(auto& p : popularity) {
        p = MAX_POPULARITY - p;
    }
    return {df.Item, popularity};
}

std::unordered_map<std::string, double> getQuantityDict(const DataFrame& df) {
    std::unordered_map<std::string, double> quantity_dict;
    for(size_t i = 0; i < df.Item.size(); i++) {
        quantity_dict[df.Item[i]] = df.Quantity[i];
    }
    return quantity_dict;
}

std::string sampleRandomProduct(const std::vector<std::string>& names,
                                const std::vector<double>& popularity,
                                const std::unordered_map<std::string, double>& quantity_dict) {
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;
    while(true) {
        double rand_p = distribution(generator);
        if(rand_p < 0 || rand_p > MAX_POPULARITY) continue;
        auto distance = [&rand_p](double p) { return std::abs(p - rand_p); };
        auto min_iter = std::min_element(popularity.begin(), popularity.end(), distance);
        std::string product = names[std::distance(popularity.begin(), min_iter)];
        if(quantity_dict.at(product) > 0) {
            return product;
        }
    }
}

std::vector<std::pair<int, int>> getPossibleProductLocs(std::vector<std::vector<Tile>>& grid) {
    std::vector<std::pair<int, int>> possible_coords;
    int row = grid.size();
    int col = grid[0].size();

    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            if (grid[r][c].type == "obstacle") {
                std::vector<std::pair<int, int>> adjacent = {{r + 1, c}, {r - 1, c}, {r, c + 1}, {r, c - 1}};
                int count = 0;
                for (int x = -WINDOW; x < WINDOW; x++) {
                    for (int y = -WINDOW; y < WINDOW; y++) {
                        if (r + x >= 0 && r + x < row && c + y >= 0 && c + y < col && grid[r + x][c + y].type == "empty") {
                            count++;
                        }
                    }
                }
                for (const auto& [adj_r, adj_c] : adjacent) {
                    if (adj_r >= 0 && adj_r < row && adj_c >= 0 && adj_c < col && grid[adj_r][adj_c].type == "empty" && (double)count / WINDOW_SIZE >= 0.4) {
                        possible_coords.emplace_back(r, c);
                    }
                }
            }
        }
    }
    return possible_coords;
}

std::pair<std::unordered_map<std::string, std::pair<int, int>>, Grid>
initProductLocations(const DataFrame& df, std::vector<std::pair<int, int>>& possible_coords, Grid& grid) {
    std::unordered_map<std::string, std::pair<int, int>> product_locations;
    std::random_device rd;
    std::mt19937 g(rd());
    
    for (const auto& product : df.Item) {
        auto coord_idx = g() % possible_coords.size();
        auto [y, x] = possible_coords[coord_idx];
        grid[y][x].type = "product";
        grid[y][x].product = product;
        product_locations[product] = {x, y};
        possible_coords.erase(possible_coords.begin() + coord_idx);
    }

    return {product_locations, grid};
}
