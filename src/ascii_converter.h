// ascii_converter.h

#ifndef ASCII_CONVERTER_H
#define ASCII_CONVERTER_H

#include "image_loader.h"

// Structure to hold ASCII art result
typedef struct {
    char* data;
    char* color_data;  // New field for color information
    int width;
    int height;
} ASCIIArt;

// Create a bloom map from the input image
Image create_bloom_map(const Image* image, float threshold);

// Apply Gaussian blur to the bloom map
Image blur_bloom_map(const Image* bloom_map, int kernel_size, float sigma);

// Apply bloom effect to the input image
Image apply_bloom_effect(const Image* image, const Image* blurred_bloom_map, float intensity);

// Convert image to ASCII art with color and bloom effect
ASCIIArt convert_to_ascii_with_color(const Image* image, const Image* edges, int ascii_width, float bloom_threshold, float bloom_intensity);

// Free ASCII art structure
void free_ascii_art(ASCIIArt* ascii_art);

// Save ASCII art to file
void save_ascii_art(const ASCIIArt* ascii_art, const char* filename);

#endif // ASCII_CONVERTER_H