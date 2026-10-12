#pragma once

enum class TileType : int
{
    Empty,
    Solid,
    Death, // Kills the player on contact (reloads the level)
};
