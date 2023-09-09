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
    
    parser.add_argument("--nagents", type = int, default = 2500,
                        help="Number of agents (Default 2500)")
    
    args = parser.parse_args()

    runSimulation(args.layout, args.storedata, args.destination, agent_num = args.nagents)

if __name__ == '__main__':
    main()