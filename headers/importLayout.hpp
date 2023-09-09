#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"

int DEFAULT_DIMS[2] = {50, 50}; 
int RESOLUTION = 3;
double INDIVIDUAL_RADIUS = 0.2; 
double SAFETY_ZONE = 1;
int DIM_UNITS[2] = {DEFAULT_DIMS[0] * RESOLUTION, DEFAULT_DIMS[1] * RESOLUTION};

void modifyDims(int new_dims[2]) {
    DEFAULT_DIMS[0] = new_dims[0];
    DEFAULT_DIMS[1] = new_dims[1];
    DIM_UNITS[0] = DEFAULT_DIMS[0] * RESOLUTION;
    DIM_UNITS[1] = DEFAULT_DIMS[1] * RESOLUTION;
}

void setParams() {
    DIM_UNITS[0] = DEFAULT_DIMS[0] * RESOLUTION; 
    DIM_UNITS[1] = DEFAULT_DIMS[1] * RESOLUTION;
}

void openImageFile(const std::string& filepath, const std::string& output_filepath) {
    int width, height, channels;
    unsigned char *img = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    
    if (!img) {
        std::cerr << "Error loading the image." << std::endl;
        return;
    }

    unsigned char *resized_img = (unsigned char *)malloc(DIM_UNITS[0] * DIM_UNITS[1] * channels);
    stbir_resize_uint8(img, width, height, 0, resized_img, DIM_UNITS[0], DIM_UNITS[1], 0, channels);
    
    stbi_write_png(output_filepath.c_str(), DIM_UNITS[0], DIM_UNITS[1], channels, resized_img, DIM_UNITS[0] * channels);
    
    stbi_image_free(img);
    free(resized_img);
}
