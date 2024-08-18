# Makefile for ASCII Art Generator

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lm

# Source files
SRCS = src/main.c src/image_loader.c src/gaussian_blur.c src/edge_detection.c src/ascii_converter.c src/utils.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = ascii_generator

# Include directory
INCLUDES = -Iinclude

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean