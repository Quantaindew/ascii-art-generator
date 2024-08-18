// image_loader.h

#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <stdint.h>

typedef struct {
    int width;
    int height;
    int channels;
    uint8_t* data;
} Image;

Image load_image(const char* filename);
void free_image(Image* img);
Image create_image(int width, int height, int channels);
Image resize_image(const Image* src, int new_width, int new_height);
float get_pixel(const Image* img, int x, int y, int channel);
void set_pixel(Image* img, int x, int y, int channel, float value);

#endif // IMAGE_LOADER_H