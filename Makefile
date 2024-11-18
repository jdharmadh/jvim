# Compiler and flags
CC = gcc
CFLAGS = -std=c17 -Wall -g

# Target executable
TARGET = jvim

# Source files
SRC = jvim.c

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean up
clean:
	rm -rf $(TARGET) jvim.dSYM

.PHONY: all clean
