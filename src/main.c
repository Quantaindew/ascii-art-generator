#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "image_loader.h"
#include "gaussian_blur.h"
#include "edge_detection.h"
#include "ascii_converter.h"
#include "utils.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define DEFAULT_OUTPUT_WIDTH 100

void print_usage(const char* program_name) {
    printf("Usage: %s <input_image> [output_width] [--color|-c]\n", program_name);
    printf("  input_image: Path to the input image file\n");
    printf("  output_width: Width of the output ASCII art (default: %d)\n", DEFAULT_OUTPUT_WIDTH);
    printf("  --color|-c: Enable color output for console (optional)\n");
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
char* generate_ascii_wasm(unsigned char* image_data, int width, int height, int channels, int output_width, bool use_color) {
    Image img = {
        .data = image_data,
        .width = width,
        .height = height,
        .channels = channels
    };

    Image blurred = apply_gaussian_blur(&img, 5, 1.0f);
    Image edges = apply_dog_edge_detection(&blurred, 5, 1.0f, 1.6f, 0.99f, 0.1f);
    EdgeInfo edge_info = apply_sobel_edge_detection(&blurred);
    Image quantized_directions = quantize_edge_direction(&edge_info.direction);
    ASCIIArt ascii_art = convert_to_ascii_with_color(&blurred, &quantized_directions, output_width);

    // Allocate memory for the result string
    char* result = (char*)malloc(strlen(use_color ? ascii_art.color_data : ascii_art.data) + 1);
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Copy the appropriate ASCII art data to the result string
    strcpy(result, use_color ? ascii_art.color_data : ascii_art.data);

    free_image(&blurred);
    free_image(&edges);
    free_edge_info(&edge_info);
    free_image(&quantized_directions);
    free_ascii_art(&ascii_art);

    return result;
}
#endif

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 5) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_filename = argv[1];
    int output_width = DEFAULT_OUTPUT_WIDTH;
    bool use_color = false;

    // Parse command-line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--color") == 0 || strcmp(argv[i], "-c") == 0) {
            use_color = true;
        } else {
            int width = atoi(argv[i]);
            if (width > 0) {
                output_width = width;
            } else {
                fprintf(stderr, "Error: Invalid output width\n");
                return EXIT_FAILURE;
            }
        }
    }

    // Load the image
    Image img = load_image(input_filename);
    if (!img.data) {
        fprintf(stderr, "Error: Failed to load image %s\n", input_filename);
        return EXIT_FAILURE;
    }

    // Apply Gaussian blur
    Image blurred = apply_gaussian_blur(&img, 5, 1.0f);

    // Apply edge detection
    Image edges = apply_dog_edge_detection(&blurred, 5, 1.0f, 1.6f, 0.99f, 0.1f);

    // Apply Sobel edge detection for direction information
    EdgeInfo edge_info = apply_sobel_edge_detection(&blurred);

    // Quantize edge directions
    Image quantized_directions = quantize_edge_direction(&edge_info.direction);

    // Convert to ASCII with color
    ASCIIArt ascii_art = convert_to_ascii_with_color(&blurred, &quantized_directions, output_width);
    if (ascii_art.data == NULL || ascii_art.color_data == NULL) {
        fprintf(stderr, "Error: Failed to convert image to ASCII art\n");
        // Clean up and return
        free_image(&img);
        free_image(&blurred);
        free_image(&edges);
        free_edge_info(&edge_info);
        free_image(&quantized_directions);
        return EXIT_FAILURE;
    }

    // Generate output filename
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s_ascii.txt", input_filename);

    // Save ASCII art (always saves non-color version)
    save_ascii_art(&ascii_art, output_filename);

    printf("ASCII art saved to %s\n", output_filename);

    // Print ASCII art to console (use color if specified)
    printf("%s", use_color ? ascii_art.color_data : ascii_art.data);

    // Clean up
    free_image(&img);
    free_image(&blurred);
    free_image(&edges);
    free_edge_info(&edge_info);
    free_image(&quantized_directions);
    free_ascii_art(&ascii_art);

    return EXIT_SUCCESS;
}