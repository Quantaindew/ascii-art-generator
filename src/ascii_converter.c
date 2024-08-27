// ascii_converter.c

#include "ascii_converter.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INTENSITY 255

// ASCII characters for different intensity levels (from darkest to brightest)
const char *ASCII_CHARS = " .:coP0?@\xe2\x96\xa0";  // UTF-8 encoding for ■
const int ASCII_CHARS_LENGTH = 10;

// ASCII characters for edges
const char *EDGE_CHARS = "|-\\/";

char* get_ascii_char(float intensity, int is_edge, int edge_direction) {
    static char buffer[4];  // Buffer to hold multi-byte characters
    if (is_edge) {
        buffer[0] = EDGE_CHARS[edge_direction];
        buffer[1] = '\0';
    } else {
        int index = (int)(intensity * (ASCII_CHARS_LENGTH - 1));
        if (index == ASCII_CHARS_LENGTH - 1) {
            // Handle the multi-byte ■ character
            strcpy(buffer, "\xe2\x96\xa0");
        } else {
            buffer[0] = ASCII_CHARS[index];
            buffer[1] = '\0';
        }
    }
    return buffer;
}

char* get_color_code(float r, float g, float b) {
    static char color_code[20];
    snprintf(color_code, sizeof(color_code), "\x1b[38;2;%d;%d;%dm", 
             (int)(r * 255), (int)(g * 255), (int)(b * 255));
    return color_code;
}

ASCIIArt convert_to_ascii_with_color(const Image* image, const Image* edges, int ascii_width) {
    ASCIIArt ascii_art;
    ascii_art.width = ascii_width;
    ascii_art.height = (int)((float)image->height / image->width * ascii_width * 0.5f);

    // Allocate more space for potential multi-byte characters and color codes
    ascii_art.data = (char *)safe_malloc(ascii_art.width * ascii_art.height * 4 + ascii_art.height + 1);
    ascii_art.color_data = (char *)safe_malloc(ascii_art.width * ascii_art.height * 30 + ascii_art.height + 1);

    float scale_x = (float)image->width / ascii_width;
    float scale_y = (float)image->height / ascii_art.height;

    int data_index = 0;
    int color_index = 0;
    for (int y = 0; y < ascii_art.height; y++) {
        for (int x = 0; x < ascii_width; x++) {
            int image_x = (int)(x * scale_x);
            int image_y = (int)(y * scale_y);

            if (image_x < image->width && image_y < image->height) {
                float intensity = 0;
                float r = 0, g = 0, b = 0;
                for (int c = 0; c < image->channels; c++) {
                    float pixel = get_pixel(image, image_x, image_y, c);
                    intensity += pixel;
                    if (c == 0) r = pixel;
                    if (c == 1) g = pixel;
                    if (c == 2) b = pixel;
                }
                intensity /= image->channels;

                int is_edge = (int)get_pixel(edges, image_x, image_y, 0);
                int edge_direction = is_edge ? (int)(get_pixel(edges, image_x, image_y, 0) * 4) % 4 : 0;

                char* ascii_char = get_ascii_char(intensity, is_edge, edge_direction);
                char* color_code = get_color_code(r, g, b);

                strncpy(&ascii_art.data[data_index], ascii_char, 4);
                data_index += strlen(ascii_char);

                strncpy(&ascii_art.color_data[color_index], color_code, 20);
                color_index += strlen(color_code);
                strncpy(&ascii_art.color_data[color_index], ascii_char, 4);
                color_index += strlen(ascii_char);
                strncpy(&ascii_art.color_data[color_index], "\x1b[0m", 5);
                color_index += 4;
            } else {
                // Handle out-of-bounds case
                ascii_art.data[data_index++] = ' ';
                strncpy(&ascii_art.color_data[color_index], " ", 2);
                color_index += 1;
            }
        }
        ascii_art.data[data_index++] = '\n';
        ascii_art.color_data[color_index++] = '\n';
    }
    ascii_art.data[data_index] = '\0';
    ascii_art.color_data[color_index] = '\0';

    return ascii_art;
}

void free_ascii_art(ASCIIArt *ascii_art) {
    if (ascii_art->data) {
        free(ascii_art->data);
        ascii_art->data = NULL;
    }
    if (ascii_art->color_data) {
        free(ascii_art->color_data);
        ascii_art->color_data = NULL;
    }
    ascii_art->width = ascii_art->height = 0;
}

void save_ascii_art(const ASCIIArt *ascii_art, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        error_exit("Error opening file %s for writing", filename);
    }

    // Always save the non-color version (ascii_art->data) to the file
    fputs(ascii_art->data, file);
    fclose(file);
}