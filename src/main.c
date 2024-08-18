// main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_loader.h"
#include "gaussian_blur.h"
#include "edge_detection.h"
#include "ascii_converter.h"
#include "utils.h"

#define DEFAULT_OUTPUT_WIDTH 100

void print_usage(const char* program_name) {
    printf("Usage: %s <input_image> [output_width]\n", program_name);
    printf("  input_image: Path to the input image file\n");
    printf("  output_width: Width of the output ASCII art (default: %d)\n", DEFAULT_OUTPUT_WIDTH);
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_filename = argv[1];
    int output_width = (argc == 3) ? atoi(argv[2]) : DEFAULT_OUTPUT_WIDTH;

    if (output_width <= 0) {
        fprintf(stderr, "Error: Invalid output width\n");
        return EXIT_FAILURE;
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

    // Convert to ASCII
    ASCIIArt ascii_art = convert_to_ascii(&blurred, &quantized_directions, output_width);

    // Generate output filename
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s_ascii.txt", input_filename);

    // Save ASCII art
    save_ascii_art(&ascii_art, output_filename);

    printf("ASCII art saved to %s\n", output_filename);

    // Clean up
    free_image(&img);
    free_image(&blurred);
    free_image(&edges);
    free_edge_info(&edge_info);
    free_image(&quantized_directions);
    free_ascii_art(&ascii_art);

    return EXIT_SUCCESS;
}