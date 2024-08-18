// image_loader.c

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../include/stb_image_resize2.h"
#include "image_loader.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

Image load_image(const char* filename) {
    Image img;
    img.data = stbi_load(filename, &img.width, &img.height, &img.channels, 0);
    
    if (!img.data) {
        error_exit("Failed to load image: %s", filename);
    }
    
    return img;
}

void free_image(Image* img) {
    if (img->data) {
        stbi_image_free(img->data);
        img->data = NULL;
    }
    img->width = img->height = img->channels = 0;
}

Image create_image(int width, int height, int channels) {
    Image img;
    img.width = width;
    img.height = height;
    img.channels = channels;
    img.data = (uint8_t*)safe_malloc(width * height * channels);
    
    return img;
}

Image resize_image(const Image* src, int new_width, int new_height) {
    Image dst = create_image(new_width, new_height, src->channels);
    
    stbir_resize_uint8_linear(
        src->data, src->width, src->height, 0,
        dst.data, dst.width, dst.height, 0,
        src->channels);
    
    return dst;
}

float get_pixel(const Image* img, int x, int y, int channel) {
    if (x < 0 || x >= img->width || y < 0 || y >= img->height || channel < 0 || channel >= img->channels) {
        return 0.0f;
    }
    
    return img->data[(y * img->width + x) * img->channels + channel] / 255.0f;
}

void set_pixel(Image* img, int x, int y, int channel, float value) {
    if (x < 0 || x >= img->width || y < 0 || y >= img->height || channel < 0 || channel >= img->channels) {
        return;
    }
    
    img->data[(y * img->width + x) * img->channels + channel] = (uint8_t)(value * 255.0f);
}