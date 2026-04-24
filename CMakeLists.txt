cmake_minimum_required(VERSION 3.21)

project(GameEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Direct final artifacts to clean locations within the build tree
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set debug/release compile and link options globally
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        add_compile_options(/Zi /Od)
        add_link_options(/DEBUG)
    else()
        add_compile_options(-g -O0)
    endif()
endif()

# Find required packages
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# ---------- Engine (object library) ----------
# OBJECT instead of STATIC ensures all translation units are linked into game
# executables. This is required for our self-registration pattern (static
# initializers in Entity.cpp, GridEntity.cpp, etc.) — a STATIC library would
# let the linker strip TUs that don't resolve any symbols, silently dropping
# the type registrations.
file(GLOB_RECURSE ENGINE_SOURCES "src/Engine/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "src/Engine/*.h")

add_library(Engine OBJECT ${ENGINE_SOURCES} ${ENGINE_HEADERS})

target_include_directories(Engine PUBLIC src/Engine)
target_include_directories(Engine PUBLIC src)
target_link_libraries(Engine PUBLIC glfw OpenGL::GL)

# ---------- Game projects ----------
# To add a new game project, just add another entry below.

# Snake
file(GLOB_RECURSE SNAKE_SOURCES "src/Snake/*.cpp")
add_executable(Snake ${SNAKE_SOURCES})
target_include_directories(Snake PRIVATE src/Snake)
target_link_libraries(Snake PRIVATE Engine)
set_target_properties(Snake PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Snake)

# Chess
file(GLOB_RECURSE CHESS_SOURCES "src/Chess/*.cpp")
add_executable(Chess ${CHESS_SOURCES})
target_include_directories(Chess PRIVATE src/Chess)
target_link_libraries(Chess PRIVATE Engine)
set_target_properties(Chess PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Chess)

# Platformer
file(GLOB_RECURSE PLATFORMER_SOURCES "src/Platformer/*.cpp")
add_executable(Platformer ${PLATFORMER_SOURCES})
target_include_directories(Platformer PRIVATE src/Platformer)
target_link_libraries(Platformer PRIVATE Engine)
set_target_properties(Platformer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Platformer)

# ---------- Tests ----------
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

file(GLOB_RECURSE TEST_SOURCES "Tests/*.cpp")
add_executable(Tests ${TEST_SOURCES})
target_link_libraries(Tests PRIVATE Engine gtest gtest_main)
set_target_properties(Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Tests)
gtest_discover_tests(Tests)