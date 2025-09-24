cmake_minimum_required(VERSION 3.10)

project(GameEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Find required packages
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Collect all source files automatically
file(GLOB_RECURSE ENGINE_SOURCES "src/Engine/*.cpp")
file(GLOB_RECURSE GAME_SOURCES "src/Game/*.cpp")

# Create the executable
add_executable(Game 
    ${ENGINE_SOURCES}
    ${GAME_SOURCES}
)

# Set include directories
target_include_directories(Game SYSTEM PRIVATE src/Engine)
target_include_directories(Game PRIVATE src/Game)
target_include_directories(Game PRIVATE src)

# Link libraries
target_link_libraries(Game PRIVATE glfw OpenGL::GL)

# Enable debug symbols and disable optimizations for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        target_compile_options(Game PRIVATE /Zi /Od)
        target_link_options(Game PRIVATE /DEBUG)
    else()
        target_compile_options(Game PRIVATE -g -O0)
    endif()
endif()