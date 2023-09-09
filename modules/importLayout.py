import ezdxf
import os
import numpy as np
import matplotlib.pyplot as plt 
from PIL import Image

DEFAULT_DIMS = (50, 50)     # Default dimensions in meters
RESOLUTION = 3
INDIVIDUAL_RADIUS = 0.2     # Average individual radius
SAFETY_ZONE = 1
DIM_UNITS = (2 * DEFAULT_DIMS[0] * RESOLUTION // 2, 2 * DEFAULT_DIMS[1] * RESOLUTION // 2)

def modifyDims(new_dims): 
    global DEFAULT_DIMS
    DEFAULT_DIMS = new_dims
    
def setParams(): 
    global DIM_UNITS
    DIM_UNITS = (2 * DEFAULT_DIMS[0] * RESOLUTION // 2, 2 * DEFAULT_DIMS[1] * RESOLUTION // 2)
    global INDIVIDUAL_RADIUS
    INDIVIDUAL_RADIUS *= RESOLUTION

def openImageFile(filepath): 
    img = Image.open(filepath).convert('L')
    img_resized = img.resize(DIM_UNITS)
    return img_resized
            
def DXFToPng(filepath, destpath): 
    doc = ezdxf.readfile(filepath)
    fig = plt.figure()
    ax = fig.add_axes([0, 0, 1, 1])
    ax.axis('off')
    doc.modelspace().plot(ax = ax)
    temppath = os.path.join(destpath, 'temp.png')
    plt.savefig(temppath, dpi = 300, bbox_inches = 'tight', pad_inches = 0)