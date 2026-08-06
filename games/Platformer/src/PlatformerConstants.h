#pragma once
#include <Engine.h>

namespace PlatformerConstants
{
    // Player properties
    static constexpr Engine::Vec2 PLAYER_SIZE{0.4f, 0.4f};
    static constexpr float PLAYER_SPEED = 9.0f;
    static constexpr float PLAYER_JUMP_FORCE = 12.0f;
    static constexpr float PLAYER_GRAVITY = 34.0f;
    static constexpr float PLAYER_COYOTE_TIME = 0.1f; // Time after leaving a platform during which the player can still jump
    static constexpr float PLAYER_WALL_COYOTE_TIME = 0.1f; // Time after leaving a wall during which the player can still wall jump
    static constexpr float PLAYER_JUMP_BUFFER_TIME = 0.1f; // Time before landing during which a jump input is buffered
    static constexpr float PLAYER_MIN_JUMP_TIME = 0.15f; // Minimum time the player must jump before they can cut the jump short by releasing the jump button
    static constexpr float PLAYER_WALL_JUMP_LOCK_TIME = 0.33f; // Time during which the player is locked in a wall jump
    static constexpr float PLAYER_ACCELERATION_COEFFICIENT = 80.0f; // How quickly the player accelerates to full speed
    static constexpr float PLAYER_DECELERATION_COEFFICIENT = 180.0f; // How quickly the player decelerates to a stop when no input is given (grounded)
    static constexpr float PLAYER_AIR_ACCELERATION_COEFFICIENT = 50.0f; // How quickly the player accelerates when airborne
    static constexpr float PLAYER_AIR_DECELERATION_COEFFICIENT = 80.0f; // How quickly the player decelerates when airborne
    static constexpr float PLAYER_WALL_HIT_DECELERATION_COEFFICIENT = 30.0f; // How quickly the player's horizontal velocity is reduced when on a wall (instead of instantaneous loss of momentum, the deceleration creates a feel of forceful impact)
    static constexpr float PLAYER_WALL_JUMP_FORCE = 13.5f; // Diagonal force applied during a wall jump
    static constexpr float PLAYER_WALL_JUMP_ANGLE = 55.0f; // Angle of wall jump in degrees (0 = pure sideways, 90 = pure up)
    static constexpr float PLAYER_MAX_FALL_SPEED = 18.0f; // Maximum downward speed in free fall (terminal velocity)
    static constexpr float PLAYER_WALL_SLIDE_MAX_SPEED = 5.0f; // Maximum downward speed when sliding against a wall
    static constexpr float PLAYER_WALL_GRAVITY_SCALE = 0.15f; // Gravity multiplier while on a wall (1.0 = normal, 0 = no gravity)
    static constexpr float PLAYER_GLIDE_MAX_SPEED = 3.0f; // Maximum downward speed when gliding
    static constexpr float PLAYER_GLIDE_GRAVITY_SCALE = 0.1f; // Gravity multiplier while gliding (1.0 = normal, 0 = no gravity)

    // Falcon properties
    static constexpr Engine::Vec2 FALCON_SIZE{0.25f, 0.25f};
    static constexpr Engine::Color FALCON_COLOR{1.0f, 1.0f, 1.0f, 1.0f};
    static constexpr Engine::Vec2 FALCON_OFFSET_FROM_PLAYER{0.2f, -0.2f}; // Falcon's position relative to the player
    static constexpr float FALCON_SPEED = 12.0f; // Falcon's movement speed
    static constexpr float FALCON_SNAP_RADIUS = 0.1f; // Distance to a flight target within which it's considered reached
    static constexpr Engine::Vec2 FALCON_GLIDE_OFFSET_FROM_PLAYER{0.0f, -0.3f}; // Falcon's position relative to the player while gliding

    // World properties
    static constexpr Engine::Color BACKGROUND_COLOR{0.5f, 0.7f, 1.0f, 1.0f};
    static constexpr Engine::Color PLAYER_COLOR{1.0f, 0.0f, 0.0f, 1.0f};
    static constexpr Engine::Color STATIC_TILE_COLOR{0.3f, 0.3f, 0.3f, 1.0f};
    static constexpr Engine::Color DEATH_TILE_COLOR{0.7f, 0.1f, 0.1f, 1.0f};
    static constexpr int GRID_COLS = 30;
    static constexpr int GRID_ROWS = 20;
    static constexpr Engine::Vec2 GRID_WORLD_SIZE{GRID_COLS, GRID_ROWS};

    // Cursor properties
    static constexpr Engine::Color CURSOR_COLOR{1.0f, 1.0f, 1.0f, 1.0f};
    static constexpr float CURSOR_RADIUS = 0.05f;
}
