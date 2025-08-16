# Simple Makefile for the Game Engine
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -Isrc
TARGET = Game
BUILD_DIR = build

# Source files
ENGINE_SRC = $(wildcard src/Engine/**/*.cpp) $(wildcard src/Engine/*.cpp)
GAME_SRC = $(wildcard src/Game/**/*.cpp) $(wildcard src/Game/*.cpp)
MAIN_SRC = src/main.cpp

# Object files
ENGINE_OBJ = $(BUILD_DIR)/Engine.o
GAME_OBJ = $(BUILD_DIR)/Game.o
MAIN_OBJ = $(BUILD_DIR)/main.o

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Build the executable
$(TARGET): $(ENGINE_OBJ) $(GAME_OBJ) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$(TARGET) $(ENGINE_OBJ) $(GAME_OBJ) $(MAIN_OBJ)

# Compile Engine
$(ENGINE_OBJ): $(ENGINE_SRC)
	$(CXX) $(CXXFLAGS) -c $(ENGINE_SRC) -o $(ENGINE_OBJ)

# Compile Game
$(GAME_OBJ): $(GAME_SRC)
	$(CXX) $(CXXFLAGS) -c $(GAME_SRC) -o $(GAME_OBJ)

# Compile main
$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -c $(MAIN_SRC) -o $(MAIN_OBJ)

# Clean build files
clean:
	rmdir /S /Q $(BUILD_DIR)

# Run the game
run: all
	./$(BUILD_DIR)/$(TARGET)

.PHONY: all clean run