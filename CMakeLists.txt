cmake_minimum_required(VERSION 3.10)

project(GameEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all source files automatically
file(GLOB_RECURSE ENGINE_SOURCES "src/Engine/*.cpp")
file(GLOB_RECURSE GAME_SOURCES "src/Game/*.cpp")

set(MAIN_SOURCES
    src/main.cpp
)

# Create the executable
add_executable(Game 
    ${MAIN_SOURCES}
    ${ENGINE_SOURCES}
    ${GAME_SOURCES}
)

# Set include directories
target_include_directories(Game PRIVATE src)

# Link libraries