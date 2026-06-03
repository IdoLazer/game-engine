#pragma once
#include <Engine.h>

namespace PlatformerConstants
{
    // Player properties
    static const Engine::Vec2 PLAYER_SIZE{0.5f, 0.5f};
    static const Engine::Vec2 PLAYER_START_POSITION{1.0f, 2.0f};
    static const float PLAYER_SPEED = 12.0f;
    static const float PLAYER_JUMP_FORCE = 18.0f;
    static const float PLAYER_GRAVITY = 45.0f;
    static const float PLAYER_COYOTE_TIME = 0.1f; // Time after leaving a platform during which the player can still jump
    static const float PLAYER_JUMP_BUFFER_TIME = 0.1f; // Time before landing during which a jump input is buffered
    static const float PLAYER_MIN_JUMP_TIME = 0.15f; // Minimum time the player must jump before they can cut the jump short by releasing the jump button
    static const float PLAYER_WALL_JUMP_LOCK_TIME = 0.25f; // Time during which the player is locked in a wall jump
    static const float PLAYER_ACCELERATION_COEFFICIENT = 150.0f; // How quickly the player accelerates to full speed
    static const float PLAYER_DECELERATION_COEFFICIENT = 160.0f; // How quickly the player decelerates to a stop when no input is given
    static const float PLAYER_WALL_JUMP_FORCE = 15.0f; // Diagonal force applied during a wall jump
    static const float PLAYER_WALL_JUMP_ANGLE = 50.0f; // Angle of wall jump in degrees (0 = pure sideways, 90 = pure up)

    // World properties
    static const Engine::Color BACKGROUND_COLOR{0.5f, 0.7f, 1.0f, 1.0f};
    static const Engine::Color PLAYER_COLOR{1.0f, 0.0f, 0.0f, 1.0f};
    static const Engine::Color TILE_COLOR{0.3f, 0.3f, 0.3f, 1.0f};
    static const std::vector<std::vector<int>> WORLD_GRID = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},  // y= 0
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},  // y= 1
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},  // y= 2
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},  // y= 3
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},  // y= 4
        {1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1},  // y= 5
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},  // y= 6
        {1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1},  // y= 7
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},  // y= 8
        {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1},  // y= 9
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},  // y=10
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},  // y=11
    };
    static const Engine::Vec2 GRID_WORLD_SIZE{(int)WORLD_GRID[0].size(), (int)WORLD_GRID.size()}; // Must match dimensions of WORLD_GRID
}
