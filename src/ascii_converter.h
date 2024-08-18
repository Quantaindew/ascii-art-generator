// ascii_converter.h

#ifndef ASCII_CONVERTER_H
#define ASCII_CONVERTER_H

#include "image_loader.h"

// Structure to hold ASCII art result
typedef struct {
    char* data;
    int width;
    int height;
} ASCIIArt;

// Convert image to ASCII art
ASCIIArt convert_to_ascii(const Image* image, const Image* edges, int ascii_width);

// Free ASCII art structure
void free_ascii_art(ASCIIArt* ascii_art);

// Save ASCII art to file
void save_ascii_art(const ASCIIArt* ascii_art, const char* filename);

#endif // ASCII_CONVERTER_H