#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <cmath>
#include <algorithm>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

const int GRID_WIDTH = 500; 
const int GRID_HEIGHT = 500;
const int PIXEL_THRESHOLD = 250;

struct Color {
    uint8_t r, g, b;
};

const std::unordered_map<std::string, Color> color_map = {
    {"empty", {255, 255, 255}},
    {"enter", {255, 255, 255}},
    {"obstacle", {0, 0, 0}},
    {"wall", {127, 127, 127}},
    {"agent", {255, 0, 0}},
    {"product", {20, 245, 237}},
    {"agent_product", {255, 0, 0}},
    {"agent_return", {0, 0, 255}}
};

double euclideanDist(const std::pair<int, int>& start, const std::pair<int, int>& end) {
    return sqrt(pow(end.first - start.first, 2) + pow(end.second - start.second, 2));
}

struct Tile {
    int x, y;
    std::pair<int, int> loc;
    std::string type;
    Tile(int x, int y, const std::string& t) : x(x), y(y), type(t), loc(std::make_pair(x, y)) {}
    bool isObstacle() const {
        return type != "empty" && type != "enter";
    }
};

std::string getType(const uint8_t* data, int width, int x, int y) {
    int index = (y * width + x) * 3;
    if (data[index] < PIXEL_THRESHOLD) return "obstacle";
    return "empty";
}

class Grid {
    std::vector<std::vector<Tile>> grid;
public:
    Grid(const std::string& imagePath) {
        int width, height, channels;
        uint8_t* data = stbi_load(imagePath.c_str(), &width, &height, &channels, 3);
        for (int y = 0; y < height; ++y) {
            std::vector<Tile> row;
            for (int x = 0; x < width; ++x) {
                row.emplace_back(x, y, getType(data, width, x, y));
            }
            grid.push_back(row);
        }
        stbi_image_free(data);
    }
    
    bool inBounds(int x, int y) const {
        return x >= 0 && x < grid[0].size() && y >= 0 && y < grid.size();
    }

    std::string typeAt(int x, int y) const {
        return inBounds(x, y) ? grid[y][x].type : "";
    }

    void setTypeAt(int x, int y, const std::string& type) {
        if (inBounds(x, y)) {
            grid[y][x].type = type;
        }
    }

    std::vector<Tile>& operator[](int index) {
        return grid[index];
    }

    const std::vector<Tile>& operator[](int index) const {
        return grid[index];
    }
};