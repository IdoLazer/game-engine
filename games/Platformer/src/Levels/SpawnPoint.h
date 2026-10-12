#pragma once

#include "LevelRegion.h"

#include <optional>
#include <string>

// A named region of a level where the player can appear.
class SpawnPoint : public LevelRegion
{
    DECLARE_TYPE(SpawnPoint, LevelRegion)

// --- Public Interface ---
public:
    const std::string &GetName() const { return m_name; }

    // The cell to appear in. Arriving through an exit, it is the region's cell nearest `exitCell`,
    // so a tall region keeps the row and a wide one the column. Otherwise it is the middle of the
    // region's bottom row.
    Engine::Vec2 GetSpawnCell(const std::optional<Engine::Vec2> &exitCell) const;

// --- Configuration (data-driven via type registry) ---
private:
    std::string m_name;
};
