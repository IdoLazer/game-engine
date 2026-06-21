#pragma once

enum class TileType : int
{
    Empty = 0,
    Solid = 1,
    EntrySpawn = 2,       // Spawn point when entering from previous level
    ReturnSpawn = 3,      // Spawn point when returning from next level
    NextLevel = 4,        // Trigger: advance to next level
    PreviousLevel = 5,    // Trigger: return to previous level
    DefaultSpawn = 6,     // Spawn point used when loading a level with no row context (e.g. reload, first load)
};
