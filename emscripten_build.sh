#!/bin/bash

# Emscripten build script for ASCII Art Generator

# Ensure the Emscripten compiler is in your PATH
if ! command -v emcc &> /dev/null
then
    echo "emcc could not be found. Please ensure Emscripten is installed and properly set up."
    exit 1
fi

# Source files
#!/bin/bash

SRCS="src/main.c src/image_loader.c src/gaussian_blur.c src/edge_detection.c src/ascii_converter.c src/utils.c"

# Output file
OUTPUT="ascii_generator.js"

# Compile
emcc $SRCS -o ascii_generator.wasm \
    -s WASM=1 \
    -s STANDALONE_WASM=1 \
    -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
    -s EXPORTED_FUNCTIONS='["_main"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=16MB \
    -s "ASYNCIFY=1" \
    -O3 \
    -I include

echo "Build complete. Output: $OUTPUT"