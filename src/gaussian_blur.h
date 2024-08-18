// gaussian_blur.h

#ifndef GAUSSIAN_BLUR_H
#define GAUSSIAN_BLUR_H

#include "image_loader.h"

// Apply Gaussian blur to an image
Image apply_gaussian_blur(const Image* src, int kernel_size, float sigma);

// Create a 1D Gaussian kernel
float* create_gaussian_kernel(int kernel_size, float sigma);

// Apply a 1D convolution (used for separable Gaussian blur)
Image apply_1d_convolution(const Image* src, const float* kernel, int kernel_size, int direction);

#endif // GAUSSIAN_BLUR_H