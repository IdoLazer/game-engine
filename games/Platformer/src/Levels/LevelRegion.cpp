#include "LevelRegion.h"
#include "TileGrid.h"

#include <iostream>
#include <optional>

using namespace Engine;

// --- Type Registration ---

BEGIN_TYPE_REGISTER(LevelRegion)
    REGISTER_PROPERTY(std::string, Anchor, &LevelRegion::m_anchor)
END_TYPE_REGISTER()

// --- Public Interface ---

bool LevelRegion::Place(const TileGrid &tiles)
{
    std::optional<Rect> bounds = tiles.FindAnchor(GetAnchor());
    if (!bounds)
    {
        std::cerr << "LevelRegion: Anchor '" << m_anchor << "' is not a character drawn in the level's picture" << std::endl;
        return false;
    }

    SetGridPosition(bounds->center);
    SetGridSize(bounds->halfExtents * 2.0f);
    return true;
}

bool LevelRegion::Contains(const Vec2 &cell) const
{
    return Rect(GetGridPosition(), GetGridSize() * 0.5f).Contains(cell);
}
