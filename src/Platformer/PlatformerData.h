#pragma once
#include <Engine.h>
#include "PlatformerConstants.h"

namespace PlatformerData
{
    using EntityInfo = Engine::Scene::EntityInfo;
    using PropertyMap = Engine::PropertyMap;

    static const std::vector<EntityInfo> ENTITY_DATA = {
        // Background
        {"Tile", PropertyMap{{"WorldPosition", Engine::Vec2(0, 0)}, {"WorldSize", PlatformerConstants::GRID_WORLD_SIZE}, {"Color", PlatformerConstants::BACKGROUND_COLOR}}},
        // Platform tiles (example layout)
        // First, create a flat ground platform at y = 11
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(1, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(2, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(3, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(4, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(5, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(6, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(7, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(8, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(9, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(10, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(11, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(12, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(13, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(14, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(16, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        // Then, create walls at columns 0 and 15
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 0)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 1)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 2)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 3)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 4)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 5)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 6)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 7)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 8)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 10)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(0, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 0)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 1)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 2)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 3)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 4)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 5)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 6)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 7)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 8)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 10)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(15, 11)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        // Now add some platforms in the middle for the player to jump on
        // Platform at y = 9 from x = 1 to x = 7
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(1, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(2, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(3, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(4, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(5, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(6, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(7, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(6, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(7, 9)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        // Platform at y = 7 from x = 8 to x = 12
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(8, 7)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(9, 7)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(10, 7)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(11, 7)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(12, 7)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        // Platform at y = 4 from x = 3 to x = 5
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(3, 4)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(4, 4)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        {"PlatformTile", PropertyMap{{"GridPosition", Engine::Vec2(5, 4)}, {"GridSize", Engine::Vec2(1, 1)}, {"Color", PlatformerConstants::TILE_COLOR}}},
        // Player
        {"Player", PropertyMap{
            // GridEntity properties
            {"GridPosition", Engine::Vec2(1, 0)},
            {"GridSize", PlatformerConstants::PLAYER_SIZE},
            {"Color", PlatformerConstants::PLAYER_COLOR},
            // Player-specific properties
            {"Speed", PlatformerConstants::PLAYER_SPEED},
            {"JumpForce", PlatformerConstants::PLAYER_JUMP_FORCE},
            {"Gravity", PlatformerConstants::PLAYER_GRAVITY},
            {"CoyoteTime", PlatformerConstants::PLAYER_COYOTE_TIME},
            {"JumpBufferTime", PlatformerConstants::PLAYER_JUMP_BUFFER_TIME},
            {"MinJumpTime", PlatformerConstants::PLAYER_MIN_JUMP_TIME}
        }},
    };
}