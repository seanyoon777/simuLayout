#include <iostream>
#include <string>
#include <cstdlib>
#include "headers/runSimulation.hpp"

int main(int argc, char* argv[]) {
    if (argc < 7) {
        std::cerr << "Usage: " << argv[0]
                  << " -l <layout path> -s <store data path> -d <destination path> [--nagents <num_agents>] [--time <time>]"
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::string layout_filepath;
    std::string df_filepath;
    std::string destination;
    int agent_num = 2500; 
    int time = TIME;  

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-l" && i + 1 < argc) {
            layout_filepath = argv[++i];
        } else if (arg == "-s" && i + 1 < argc) {
            df_filepath = argv[++i];
        } else if (arg == "-d" && i + 1 < argc) {
            destination = argv[++i];
        } else if (arg == "--nagents" && i + 1 < argc) {
            agent_num = std::stoi(argv[++i]);
        } else if (arg == "--time" && i + 1 < argc) {
            time = std::stoi(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return EXIT_FAILURE;
        }
    }

    if (layout_filepath.empty() || df_filepath.empty() || destination.empty()) {
        std::cerr << "Please provide the required arguments." << std::endl;
        return EXIT_FAILURE;
    }

    runSimulation(layout_filepath, df_filepath, destination, agent_num, time);

    return EXIT_SUCCESS;
}
