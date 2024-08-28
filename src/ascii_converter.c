// ascii_converter.c

#include "ascii_converter.h"
#include "gaussian_blur.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INTENSITY 255

// ASCII characters for different intensity levels (from darkest to brightest)
const char *ASCII_CHARS = " .:coP0?@\xe2\x96\xa0"; // UTF-8 encoding for ■
const int ASCII_CHARS_LENGTH = 10;

// ASCII characters for edges
const char *EDGE_CHARS = "|-\\/";

char *get_ascii_char(float intensity, int is_edge, int edge_direction) {
  static char buffer[4]; // Buffer to hold multi-byte characters
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

char *get_color_code(float r, float g, float b) {
  static char color_code[20];
  // Clamp RGB values to [0, 1] range
  r = clamp(r, 0.0f, 1.0f);
  g = clamp(g, 0.0f, 1.0f);
  b = clamp(b, 0.0f, 1.0f);
  snprintf(color_code, sizeof(color_code), "\x1b[38;2;%d;%d;%dm",
           (int)(r * 255), (int)(g * 255), (int)(b * 255));
  return color_code;
}

Image create_bloom_map(const Image *image, float threshold) {
  Image bloom_map = create_image(image->width, image->height, 1);

  for (int y = 0; y < image->height; y++) {
    for (int x = 0; x < image->width; x++) {
      float intensity = 0;
      for (int c = 0; c < image->channels; c++) {
        intensity += get_pixel(image, x, y, c);
      }
      intensity /= image->channels;

      float bloom_value = intensity > threshold ? intensity : 0;
      set_pixel(&bloom_map, x, y, 0, bloom_value);
    }
  }

  return bloom_map;
}

Image blur_bloom_map(const Image *bloom_map, int kernel_size, float sigma) {
  return apply_gaussian_blur(bloom_map, kernel_size, sigma);
}

Image apply_bloom_effect(const Image *image, const Image *blurred_bloom_map,
                         float intensity) {
  Image result = create_image(image->width, image->height, image->channels);

  for (int y = 0; y < image->height; y++) {
    for (int x = 0; x < image->width; x++) {
      for (int c = 0; c < image->channels; c++) {
        float original = get_pixel(image, x, y, c);
        float bloom = get_pixel(blurred_bloom_map, x, y, 0);
        float new_value = original + bloom * intensity;
        set_pixel(&result, x, y, c, new_value);
      }
    }
  }

  return result;
}

ASCIIArt convert_to_ascii_with_color(const Image *image, const Image *edges,
                                     int ascii_width, float bloom_threshold,
                                     float bloom_intensity) {
  // Create bloom map
  Image bloom_map = create_bloom_map(image, bloom_threshold);

  // Blur bloom map
  Image blurred_bloom_map = blur_bloom_map(&bloom_map, 5, 1.0f);

  // Apply bloom effect
  Image bloomed_image =
      apply_bloom_effect(image, &blurred_bloom_map, bloom_intensity);

  ASCIIArt ascii_art;
  ascii_art.width = ascii_width;
  ascii_art.height =
      (int)((float)image->height / image->width * ascii_width * 0.5f);

  // Allocate more space for potential multi-byte characters and color codes
  ascii_art.data = (char *)safe_malloc(ascii_art.width * ascii_art.height * 4 +
                                       ascii_art.height + 1);
  ascii_art.color_data = (char *)safe_malloc(
      ascii_art.width * ascii_art.height * 30 + ascii_art.height + 1);

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
        for (int c = 0; c < bloomed_image.channels; c++) {
          float pixel = get_pixel(&bloomed_image, image_x, image_y, c);
          intensity += pixel;
          if (c == 0)
            r = pixel;
          if (c == 1)
            g = pixel;
          if (c == 2)
            b = pixel;
        }
        intensity /= bloomed_image.channels;

        int is_edge = (int)get_pixel(edges, image_x, image_y, 0);
        int edge_direction =
            is_edge ? (int)(get_pixel(edges, image_x, image_y, 0) * 4) % 4 : 0;

        char *ascii_char = get_ascii_char(intensity, is_edge, edge_direction);
        char *color_code = get_color_code(r, g, b);

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

  // Clean up
  free_image(&bloom_map);
  free_image(&blurred_bloom_map);
  free_image(&bloomed_image);

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
