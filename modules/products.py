import numpy as np
import pandas as pd
import random
from scipy.stats import norm 

FACTOR_WEIGHTS = [1, .5, .5]
MAX_POPULARITY = 2.
WINDOW = 2
WINDOW_SIZE = (WINDOW * 2)**2 - 1

def readDataset(filepath): 
    df = pd.read_csv(filepath)
    return df

def getItemPopularity(col1, col2, col3): 
    popularity = list(col1 * FACTOR_WEIGHTS[0] + col2 * FACTOR_WEIGHTS[1] + col3 * FACTOR_WEIGHTS[2])
    return popularity
    
def getPopularityList(df): 
    df['Norm Rating'] = df['Rating'].astype(float) / 5
    df['Regular Price'] = df['Regular Price'].astype(float)
    df['Discount Price'] = df['Discount Price'].astype(float)
    df['Norm Discount'] = (df['Regular Price'] - df['Discount Price']) / df['Regular Price']
    df['Quantity'] = df['Quantity'].astype(float)
    df['Norm Quantity'] = (df['Quantity'] - df['Quantity'].min()) / (df['Quantity'].max() - df['Quantity'].min())
    popularity = getItemPopularity(df['Norm Discount'], df['Norm Quantity'], df['Norm Rating'])
    popularity = [MAX_POPULARITY - p for p in popularity]
    names = list(df['Item'])
    return (names, popularity)

def getQuantityDict(df): 
    quantity_dict = {name: q for (name, q) in zip(list(df['Item']), list(df['Quantity']))}
    return quantity_dict

def sampleRandomProduct(names, popularity, quantity_dict): 
    rand_p = -np.inf
    product = ""
    while rand_p < 0 or rand_p > MAX_POPULARITY: 
        rand_p = norm.rvs()
        closest_idx = np.argmin([abs(p - rand_p) for p in popularity])
        product = names[closest_idx]
        if quantity_dict[product] <= 0: 
            rand_p = -np.inf
    return product

def getCategoryDict(df): 
    category_dict = {}
    for idx, row in df.iterrows(): 
        category = row['Product Category']
        item = row['Item']
        category_dict[item] = category
    return category_dict

def getPossibleProductLocs(grid): 
    row, col = grid.shape
    possible_coords = []
    for r in range(row): 
        for c in range(col): 
            if grid[r][c].type == "obstacle": 
                adjacent = [(r + 1, c), (r - 1, c), (r, c + 1), (r, c - 1)]
                count = 0
                for x in range(-WINDOW, WINDOW, 1): 
                    for y in range(-WINDOW, WINDOW, 1): 
                        if grid[r + x][c + y].type == "empty": 
                            count += 1
                for x, y in adjacent: 
                    if grid[x][y].type == "empty" and count / WINDOW_SIZE >= 0.4: 
                        possible_coords.append((r, c))
    return possible_coords
    
def initProductLocations(df, possible_coords, grid): 
    product_locations = {}
    for product in df["Item"]: 
        y, x = random.choice(possible_coords)
        grid[y][x].type = "product"
        grid[y][x].product = product
        product_locations[product] = (x, y)
        possible_coords.remove((y, x))
    return product_locations, grid