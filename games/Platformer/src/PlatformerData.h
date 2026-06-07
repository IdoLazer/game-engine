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
        // World
        {"PlatformerWorld", PropertyMap{
            {"StaticTileColor", PlatformerConstants::STATIC_TILE_COLOR},
            {"LevelEndColor", PlatformerConstants::LEVEL_END_COLOR}
        }},
        // Player
        {"Player", PropertyMap{
            // GridEntity properties
            {"GridSize", PlatformerConstants::PLAYER_SIZE},
            {"Color", PlatformerConstants::PLAYER_COLOR},
            // Player-specific properties
            {"Speed", PlatformerConstants::PLAYER_SPEED},
            {"AccelerationCoefficient", PlatformerConstants::PLAYER_ACCELERATION_COEFFICIENT},
            {"DecelerationCoefficient", PlatformerConstants::PLAYER_DECELERATION_COEFFICIENT},
            {"JumpForce", PlatformerConstants::PLAYER_JUMP_FORCE},
            {"Gravity", PlatformerConstants::PLAYER_GRAVITY},
            {"CoyoteTime", PlatformerConstants::PLAYER_COYOTE_TIME},
            {"JumpBufferTime", PlatformerConstants::PLAYER_JUMP_BUFFER_TIME},
            {"MinJumpTime", PlatformerConstants::PLAYER_MIN_JUMP_TIME},
            {"WallJumpLockTime", PlatformerConstants::PLAYER_WALL_JUMP_LOCK_TIME},
            {"WallJumpForce", PlatformerConstants::PLAYER_WALL_JUMP_FORCE},
            {"WallJumpAngle", PlatformerConstants::PLAYER_WALL_JUMP_ANGLE},
            {"MaxFallSpeed", PlatformerConstants::PLAYER_MAX_FALL_SPEED},
            {"WallSlideMaxSpeed", PlatformerConstants::PLAYER_WALL_SLIDE_MAX_SPEED},
            {"WallGravityScale", PlatformerConstants::PLAYER_WALL_GRAVITY_SCALE}
        }},
    };
}
