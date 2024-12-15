# Compiler and flags
CC = gcc
CFLAGS = -std=c17 -Wall -g

# Target executable
TARGET = jvim

# Source files
SRC = jvim.c text.c editor.c command.c

# Header files
HEADERS = layouts.h text.h colors.h editor.h command.h

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean up
clean:
	rm -rf $(TARGET) jvim.dSYM

.PHONY: all clean
