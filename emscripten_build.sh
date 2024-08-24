#!/bin/bash

# Emscripten build script for ASCII Art Generator

# Ensure the Emscripten compiler is in your PATH
if ! command -v emcc &> /dev/null
then
    echo "emcc could not be found. Please ensure Emscripten is installed and properly set up."
    exit 1
fi

# Source files
SRCS="src/main.c src/image_loader.c src/gaussian_blur.c src/edge_detection.c src/ascii_converter.c src/utils.c"

# Output files
OUTPUT="ascii_generator"

# Compile
emcc $SRCS -o $OUTPUT.js \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_malloc", "_free", "_generate_ascii_wasm"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=256MB \
    -s ASYNCIFY=1 \
    -s FORCE_FILESYSTEM=1 \
    --preload-file examples \
    -O3 \
    -I include

echo "Build complete. Output: $OUTPUT.js and $OUTPUT.wasm"