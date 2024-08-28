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
#define DEFAULT_BLOOM_THRESHOLD 0.7f
#define DEFAULT_BLOOM_INTENSITY 0.3f

void print_usage(const char* program_name) {
    printf("Usage: %s <input_image> [output_width] [--color|-c] [--edge|-e] [--bloom-threshold|-bth <value>] [--bloom-intensity|-bi <value>]\n", program_name);
    printf("  input_image: Path to the input image file\n");
    printf("  output_width: Width of the output ASCII art (default: %d)\n", DEFAULT_OUTPUT_WIDTH);
    printf("  --color|-c: Enable color output for console (optional)\n");
    printf("  --edge|-e: Enable edge detection (optional)\n");
    printf("  --bloom-threshold|-bth <value>: Set bloom threshold (default: %.2f)\n", DEFAULT_BLOOM_THRESHOLD);
    printf("  --bloom-intensity|-bi <value>: Set bloom intensity (default: %.2f)\n", DEFAULT_BLOOM_INTENSITY);
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
char* generate_ascii_wasm(unsigned char* image_data, int width, int height, int channels, int output_width, bool use_color, float bloom_threshold, float bloom_intensity) {
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
    ASCIIArt ascii_art = convert_to_ascii_with_color(&blurred, &quantized_directions, output_width, bloom_threshold, bloom_intensity);

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
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_filename = argv[1];
    int output_width = DEFAULT_OUTPUT_WIDTH;
    bool use_color = false;
    bool use_edge_detection = false;
    float bloom_threshold = DEFAULT_BLOOM_THRESHOLD;
    float bloom_intensity = DEFAULT_BLOOM_INTENSITY;

    // Parse command-line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--color") == 0 || strcmp(argv[i], "-c") == 0) {
            use_color = true;
        } else if (strcmp(argv[i], "--edge") == 0 || strcmp(argv[i], "-e") == 0) {
            use_edge_detection = true;
        } else if ((strcmp(argv[i], "--bloom-threshold") == 0 || strcmp(argv[i], "-bth") == 0) && i + 1 < argc) {
            bloom_threshold = atof(argv[++i]);
        } else if ((strcmp(argv[i], "--bloom-intensity") == 0 || strcmp(argv[i], "-bi") == 0) && i + 1 < argc) {
            bloom_intensity = atof(argv[++i]);
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

    // Initialize edges and quantized_directions as empty images
    Image edges = {0};
    Image quantized_directions = {0};

    if (use_edge_detection) {
        // Apply edge detection
        edges = apply_dog_edge_detection(&blurred, 5, 1.0f, 1.6f, 0.99f, 0.1f);
        EdgeInfo edge_info = apply_sobel_edge_detection(&blurred);
        quantized_directions = quantize_edge_direction(&edge_info.direction);
        free_edge_info(&edge_info);
    }

    // Convert to ASCII with color and bloom effect
    ASCIIArt ascii_art = convert_to_ascii_with_color(&blurred, &quantized_directions, output_width, bloom_threshold, bloom_intensity);
    if (ascii_art.data == NULL || ascii_art.color_data == NULL) {
        fprintf(stderr, "Error: Failed to convert image to ASCII art\n");
        // Clean up and return
        free_image(&img);
        free_image(&blurred);
        if (use_edge_detection) {
            free_image(&edges);
            free_image(&quantized_directions);
        }
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
    if (use_edge_detection) {
        free_image(&edges);
        free_image(&quantized_directions);
    }
    free_ascii_art(&ascii_art);

    return EXIT_SUCCESS;
}