import argparse
from modules.runSimulation import *

def main():
    parser = argparse.ArgumentParser(description='Process store data and generate gif simulation.')
    parser.add_argument("-l", "--layout", type = str, required = True,
                        help="Path to store layout file (png, jpg, or dxf).")
    
    parser.add_argument("-s", "--storedata", type = str, required = True,
                        help="Path to the store data CSV file.")
    
    parser.add_argument("-d", "--destination", type = str, required = True,
                        help="Destination path to save results.")
    
    parser.add_argument("--nagents", type = int, default = 1000,
                        help="Number of agents (Default 1000)")
    parser.add_argument("--time", type = int, default = 100,
                    help="Duration of the simulation (Default 100)")
    
    args = parser.parse_args()

    runSimulation(args.layout, args.storedata, args.destination, agent_num = args.nagents, time = args.time)

if __name__ == '__main__':
    main()