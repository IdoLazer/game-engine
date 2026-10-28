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
    bool Place(PlatformerWorld &world) override;

    const std::string &GetName() const { return m_name; }

    // '\0' unless the document gave exactly one character.
    char GetDefaultAnchor() const { return m_defaultAnchor.size() == 1 ? m_defaultAnchor[0] : '\0'; }

    // The cell to appear in. Arriving through an exit, it is the region's cell nearest `exitCell`,
    // so a tall region keeps the row and a wide one the column. Otherwise it is the cell drawn with
    // the default anchor, or the middle of the region's bottom row when the document names none.
    Engine::Vec2 GetSpawnCell(const std::optional<Engine::Vec2> &exitCell) const;

// --- Configuration (data-driven via type registry) ---
private:
    std::string m_name;
    std::string m_defaultAnchor;

// --- Private Fields ---
private:
    std::optional<Engine::Vec2> m_defaultCell;
};
