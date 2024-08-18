// gaussian_blur.c

#include "gaussian_blur.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

float* create_gaussian_kernel(int kernel_size, float sigma) {
    float* kernel = (float*)malloc(kernel_size * sizeof(float));
    float sum = 0.0f;
    int half = kernel_size / 2;

    for (int i = 0; i < kernel_size; i++) {
        float x = i - half;
        kernel[i] = exp(-(x * x) / (2 * sigma * sigma)) / (sqrt(2 * PI) * sigma);
        sum += kernel[i];
    }

    // Normalize the kernel
    for (int i = 0; i < kernel_size; i++) {
        kernel[i] /= sum;
    }

    return kernel;
}

Image apply_1d_convolution(const Image* src, const float* kernel, int kernel_size, int direction) {
    Image dst = create_image(src->width, src->height, src->channels);
    int half = kernel_size / 2;

    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            for (int c = 0; c < src->channels; c++) {
                float sum = 0.0f;
                for (int i = -half; i <= half; i++) {
                    int sx = (direction == 0) ? clamp(x + i, 0, src->width - 1) : x;
                    int sy = (direction == 1) ? clamp(y + i, 0, src->height - 1) : y;
                    float pixel = get_pixel(src, sx, sy, c);
                    sum += pixel * kernel[i + half];
                }
                set_pixel(&dst, x, y, c, sum);
            }
        }
    }

    return dst;
}

Image apply_gaussian_blur(const Image* src, int kernel_size, float sigma) {
    float* kernel = create_gaussian_kernel(kernel_size, sigma);

    // Apply horizontal blur
    Image temp = apply_1d_convolution(src, kernel, kernel_size, 0);

    // Apply vertical blur
    Image result = apply_1d_convolution(&temp, kernel, kernel_size, 1);

    // Clean up
    free_image(&temp);
    free(kernel);

    return result;
}