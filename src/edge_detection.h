// edge_detection.h

#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include "image_loader.h"

// Structure to hold edge detection results
typedef struct {
    Image magnitude;
    Image direction;
} EdgeInfo;

// Apply Difference of Gaussians (DoG) edge detection
Image apply_dog_edge_detection(const Image* src, int kernel_size, float sigma, float sigma_scale, float tau, float threshold);

// Apply Sobel edge detection
EdgeInfo apply_sobel_edge_detection(const Image* src);

// Quantize edge directions
Image quantize_edge_direction(const Image* direction);

// Free EdgeInfo structure
void free_edge_info(EdgeInfo* edge_info);

#endif // EDGE_DETECTION_H