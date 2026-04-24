#pragma once
#include <Engine.h>

namespace PlatformerConstants
{
    // Player properties
    static const Engine::Vec2 PLAYER_SIZE{0.5f, 0.5f};
    static const float PLAYER_SPEED = 10.0f;
    static const float PLAYER_JUMP_FORCE = 15.0f;
    static const float PLAYER_GRAVITY = 35.0f;
    static const float PLAYER_COYOTE_TIME = 0.1f; // Time after leaving a platform during which the player can still jump
    static const float PLAYER_JUMP_BUFFER_TIME = 0.1f; // Time before landing during which a jump input is buffered
    static const float PLAYER_MIN_JUMP_TIME = 0.15f; // Minimum time the player must jump before they can cut the jump short by releasing the jump button

    // World properties
    static const Engine::Vec2 GRID_WORLD_SIZE{16.0f, 12.0f};
    static const Engine::Color BACKGROUND_COLOR{0.5f, 0.7f, 1.0f, 1.0f};
    static const Engine::Color PLAYER_COLOR{1.0f, 0.0f, 0.0f, 1.0f};
    static const Engine::Color TILE_COLOR{0.3f, 0.3f, 0.3f, 1.0f};
}