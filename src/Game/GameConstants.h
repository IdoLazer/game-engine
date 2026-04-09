#pragma once

#include <Engine.h>

// Game Constants
namespace GameConstants
{
    // Player Settings
    const Engine::Vec2 PLAYER_SIZE{1.0f, 1.0f};
    const Engine::Color PLAYER_COLOR{1.0f, 0.0f, 0.0f, 1.0f}; // Red
    static constexpr int INITIAL_TAIL_LENGTH = 1;
    static constexpr float MOVE_SPEED = 6.0f; // Grid cells per second

    // Food Settings
    const Engine::Vec2 FOOD_SIZE{0.5f, 0.5f};
    const Engine::Color FOOD_COLOR{0.0f, 1.0f, 0.0f, 1.0f}; // Green

    // World Settings
    const Engine::Vec2 GRID_CELL_COUNT{13.0f, 10.0f};
    const Engine::Color BACKGROUND_COLOR{0.0f, 0.0f, 1.0f, 1.0f}; // Blue

    // Movement
    const Engine::Vec2 DIRECTION_UP{0.0f, -1.0f};
    const Engine::Vec2 DIRECTION_DOWN{0.0f, 1.0f};
    const Engine::Vec2 DIRECTION_LEFT{-1.0f, 0.0f};
    const Engine::Vec2 DIRECTION_RIGHT{1.0f, 0.0f};
}