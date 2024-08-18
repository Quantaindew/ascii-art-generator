// edge_detection.c

#include "edge_detection.h"
#include "gaussian_blur.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

Image apply_dog_edge_detection(const Image* src, int kernel_size, float sigma, float sigma_scale, float tau, float threshold) {
    // Apply first Gaussian blur
    Image blur1 = apply_gaussian_blur(src, kernel_size, sigma);
    
    // Apply second Gaussian blur
    Image blur2 = apply_gaussian_blur(src, kernel_size, sigma * sigma_scale);
    
    // Create output image
    Image dog = create_image(src->width, src->height, 1);  // Single channel for edge detection
    
    // Compute Difference of Gaussians
    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            float diff = 0;
            for (int c = 0; c < src->channels; c++) {
                diff += get_pixel(&blur1, x, y, c) - tau * get_pixel(&blur2, x, y, c);
            }
            diff /= src->channels;
            set_pixel(&dog, x, y, 0, diff >= threshold ? 1.0f : 0.0f);
        }
    }
    
    // Clean up
    free_image(&blur1);
    free_image(&blur2);
    
    return dog;
}

EdgeInfo apply_sobel_edge_detection(const Image* src) {
    EdgeInfo edge_info;
    edge_info.magnitude = create_image(src->width, src->height, 1);
    edge_info.direction = create_image(src->width, src->height, 1);
    
    float kernel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    float kernel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    for (int y = 1; y < src->height - 1; y++) {
        for (int x = 1; x < src->width - 1; x++) {
            float gx = 0, gy = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    float pixel = get_pixel(src, x + j, y + i, 0);  // Assume grayscale
                    gx += pixel * kernel_x[i+1][j+1];
                    gy += pixel * kernel_y[i+1][j+1];
                }
            }
            float magnitude = sqrt(gx*gx + gy*gy);
            float direction = atan2(gy, gx);
            
            set_pixel(&edge_info.magnitude, x, y, 0, magnitude);
            set_pixel(&edge_info.direction, x, y, 0, direction);
        }
    }
    
    return edge_info;
}

Image quantize_edge_direction(const Image* direction) {
    Image quantized = create_image(direction->width, direction->height, 1);
    
    for (int y = 0; y < direction->height; y++) {
        for (int x = 0; x < direction->width; x++) {
            float angle = get_pixel(direction, x, y, 0);
            float abs_angle = fabs(angle) / PI;
            int value;
            
            if ((abs_angle >= 0.0 && abs_angle < 0.05) || (abs_angle > 0.95 && abs_angle <= 1.0)) {
                value = 0;  // Vertical
            } else if (abs_angle > 0.45 && abs_angle < 0.55) {
                value = 1;  // Horizontal
            } else if ((abs_angle > 0.05 && abs_angle < 0.45 && angle > 0) ||
                       (abs_angle > 0.55 && abs_angle < 0.95 && angle < 0)) {
                value = 2;  // Diagonal 1
            } else {
                value = 3;  // Diagonal 2
            }
            
            set_pixel(&quantized, x, y, 0, value / 3.0f);  // Normalize to [0, 1]
        }
    }
    
    return quantized;
}

void free_edge_info(EdgeInfo* edge_info) {
    free_image(&edge_info->magnitude);
    free_image(&edge_info->direction);
}