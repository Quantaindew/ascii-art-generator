// utils.h

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>  // Added this line to define size_t

// Math utilities
float clamp(float value, float min, float max);
float min_float(float a, float b);
float max_float(float a, float b);
int min_int(int a, int b);
int max_int(int a, int b);
float lerp(float a, float b, float t);
float map(float value, float in_min, float in_max, float out_min, float out_max);
float deg_to_rad(float degrees);
float rad_to_deg(float radians);
int round_float(float x);

// Memory utilities
void* safe_malloc(size_t size);
void* safe_calloc(size_t num, size_t size);
void* safe_realloc(void* ptr, size_t size);

// File utilities
bool file_exists(const char* filename);
bool create_directory(const char* path);
const char* get_file_extension(const char* filename);

// String utilities
void string_to_lower(char* str);
char* trim_string(char* str);

// Error handling
void error_exit(const char* format, ...);

#endif // UTILS_H