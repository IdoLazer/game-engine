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

# Define available projects
set(AVAILABLE_PROJECTS "Game" "Chess")

# Check if TARGET_PROJECT is specified
if(NOT DEFINED TARGET_PROJECT)
    message(FATAL_ERROR "TARGET_PROJECT must be specified. Available options: ${AVAILABLE_PROJECTS}")
endif()

# Validate TARGET_PROJECT
if(NOT TARGET_PROJECT IN_LIST AVAILABLE_PROJECTS)
    message(STATUS "Received TARGET_PROJECT: '${TARGET_PROJECT}'")
    message(FATAL_ERROR "Invalid TARGET_PROJECT '${TARGET_PROJECT}'. Available options: ${AVAILABLE_PROJECTS}")
endif()

# Set up clean output directories
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/out/install")
set(PLATFORM_DIR "Windows_x64")
set(CONFIG_DIR "${CMAKE_BUILD_TYPE}")
set(FINAL_OUTPUT_DIR "${OUTPUT_DIR}/${PLATFORM_DIR}/${CONFIG_DIR}")

# Create output directory if it doesn't exist
file(MAKE_DIRECTORY ${FINAL_OUTPUT_DIR})

# Collect all source files automatically
file(GLOB_RECURSE ENGINE_SOURCES "src/Engine/*.cpp")
file(GLOB_RECURSE PROJECT_SOURCES "src/${TARGET_PROJECT}/*.cpp")

# Create the executable
add_executable(${TARGET_PROJECT}
    ${ENGINE_SOURCES}
    ${PROJECT_SOURCES}
)

# Set the output directory for the executable
set_target_properties(${TARGET_PROJECT} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${FINAL_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${FINAL_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${FINAL_OUTPUT_DIR}
)

# Set include directories
target_include_directories(${TARGET_PROJECT} SYSTEM PRIVATE src/Engine)
target_include_directories(${TARGET_PROJECT} PRIVATE src/${TARGET_PROJECT})
target_include_directories(${TARGET_PROJECT} PRIVATE src)

# Link libraries
target_link_libraries(${TARGET_PROJECT} PRIVATE glfw OpenGL::GL)

# Enable debug symbols and disable optimizations for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        target_compile_options(${TARGET_PROJECT} PRIVATE /Zi /Od)
        target_link_options(${TARGET_PROJECT} PRIVATE /DEBUG)
    else()
        target_compile_options(${TARGET_PROJECT} PRIVATE -g -O0)
    endif()
endif()