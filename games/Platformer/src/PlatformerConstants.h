#pragma once
#include <Engine.h>

namespace PlatformerConstants
{
    // Player properties
    static const Engine::Vec2 PLAYER_SIZE{0.4f, 0.4f};
    static const float PLAYER_SPEED = 9.0f;
    static const float PLAYER_JUMP_FORCE = 12.0f;
    static const float PLAYER_GRAVITY = 34.0f;
    static const float PLAYER_COYOTE_TIME = 0.1f; // Time after leaving a platform during which the player can still jump
    static const float PLAYER_WALL_COYOTE_TIME = 0.1f; // Time after leaving a wall during which the player can still wall jump
    static const float PLAYER_JUMP_BUFFER_TIME = 0.1f; // Time before landing during which a jump input is buffered
    static const float PLAYER_MIN_JUMP_TIME = 0.15f; // Minimum time the player must jump before they can cut the jump short by releasing the jump button
    static const float PLAYER_WALL_JUMP_LOCK_TIME = 0.27f; // Time during which the player is locked in a wall jump
    static const float PLAYER_ACCELERATION_COEFFICIENT = 80.0f; // How quickly the player accelerates to full speed
    static const float PLAYER_DECELERATION_COEFFICIENT = 180.0f; // How quickly the player decelerates to a stop when no input is given (grounded)
    static const float PLAYER_AIR_ACCELERATION_COEFFICIENT = 50.0f; // How quickly the player accelerates when airborne
    static const float PLAYER_AIR_DECELERATION_COEFFICIENT = 80.0f; // How quickly the player decelerates when airborne
    static const float PLAYER_WALL_HIT_DECELERATION_COEFFICIENT = 30.0f; // How quickly the player's horizontal velocity is reduced when on a wall (instead of instantaneous loss of momentum, the deceleration creates a feel of forceful impact)
    static const float PLAYER_WALL_JUMP_FORCE = 13.5f; // Diagonal force applied during a wall jump
    static const float PLAYER_WALL_JUMP_ANGLE = 60.0f; // Angle of wall jump in degrees (0 = pure sideways, 90 = pure up)
    static const float PLAYER_MAX_FALL_SPEED = 18.0f; // Maximum downward speed in free fall (terminal velocity)
    static const float PLAYER_WALL_SLIDE_MAX_SPEED = 5.0f; // Maximum downward speed when sliding against a wall
    static constexpr float PLAYER_WALL_GRAVITY_SCALE = 0.15f; // Gravity multiplier while on a wall (1.0 = normal, 0 = no gravity)

    // World properties
    static const Engine::Color BACKGROUND_COLOR{0.5f, 0.7f, 1.0f, 1.0f};
    static const Engine::Color PLAYER_COLOR{1.0f, 0.0f, 0.0f, 1.0f};
    static const Engine::Color STATIC_TILE_COLOR{0.3f, 0.3f, 0.3f, 1.0f};
    static constexpr int GRID_COLS = 30;
    static constexpr int GRID_ROWS = 20;
    static const Engine::Vec2 GRID_WORLD_SIZE{GRID_COLS, GRID_ROWS};
}
