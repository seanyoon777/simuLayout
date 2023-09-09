import os
from random import randint
from modules.importLayout import *
from modules.products import *
from modules.agents import *
from PIL import Image, ImageSequence
from tqdm import tqdm

TIME = 1000

def generateID(n = 7): 
    range_start = 10**(n - 1)
    range_end = (10**n) - 1
    return str(randint(range_start, range_end))

def runSimulation(layout_filepath, df_filepath, destination, agent_num = 2500, time = TIME): 
    img = Image.open(layout_filepath).convert('L')
    img_resized = img.resize(DIM_UNITS)
    grid = Grid(img_resized)

    df = readDataset(df_filepath)
    names, popularity = getPopularityList(df)
    quantity_dict = getQuantityDict(df)
    category_dict = getCategoryDict(df)
    possible_coords = getPossibleProductLocs(grid)
    product_locations, grid = initProductLocations(df, possible_coords, grid)

    agents = initAgents(grid, names, popularity, quantity_dict, category_dict, product_locations, agent_num)

    frames = []

    for t in tqdm(range(time)): 
        frame = grid.plotImage() 
        frames.append(frame)
        moveAgents(grid, agents, quantity_dict, product_locations)

    output_pathname = os.path.join(destination, 'simulation_' + generateID() + '.gif')
    frames[0].save(output_pathname,
                save_all=True, append_images=frames[1:], optimize=False, duration=100, loop=0)
