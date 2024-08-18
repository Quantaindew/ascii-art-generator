// utils.c

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include "utils.h"

// Define PI if it's not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Math utilities
float clamp(float value, float min, float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

float min_float(float a, float b) {
    return (a < b) ? a : b;
}

float max_float(float a, float b) {
    return (a > b) ? a : b;
}

int min_int(int a, int b) {
    return (a < b) ? a : b;
}

int max_int(int a, int b) {
    return (a > b) ? a : b;
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float map(float value, float in_min, float in_max, float out_min, float out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float deg_to_rad(float degrees) {
    return degrees * ((float)M_PI / 180.0f);
}

float rad_to_deg(float radians) {
    return radians * (180.0f / (float)M_PI);
}

int round_float(float x) {
    return (int)(x + 0.5f);
}

// Memory utilities
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        error_exit("Memory allocation failed");
    }
    return ptr;
}

void* safe_calloc(size_t num, size_t size) {
    void* ptr = calloc(num, size);
    if (ptr == NULL) {
        error_exit("Memory allocation failed");
    }
    return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        error_exit("Memory reallocation failed");
    }
    return new_ptr;
}

// File utilities
bool file_exists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

bool create_directory(const char* path) {
    #ifdef _WIN32
        return _mkdir(path) == 0;
    #else
        return mkdir(path, 0777) == 0;
    #endif
}

const char* get_file_extension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

// String utilities
void string_to_lower(char* str) {
    for (char* p = str; *p; p++) {
        *p = tolower((unsigned char)*p);
    }
}

char* trim_string(char* str) {
    char* end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

// Error handling
void error_exit(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}