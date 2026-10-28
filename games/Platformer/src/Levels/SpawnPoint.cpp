#include "SpawnPoint.h"
#include "../PlatformerWorld.h"

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace Engine;

// --- Type Registration ---

BEGIN_TYPE_REGISTER(SpawnPoint)
    REGISTER_PROPERTY(std::string, Name, &SpawnPoint::m_name)
    REGISTER_PROPERTY(std::string, DefaultAnchor, &SpawnPoint::m_defaultAnchor)
END_TYPE_REGISTER()

// --- Public Interface ---

bool SpawnPoint::Place(PlatformerWorld &world)
{
    if (!LevelRegion::Place(world))
        return false;

    if (m_defaultAnchor.empty())
        return true;

    std::optional<Rect> bounds = world.ClaimAnchor(GetDefaultAnchor());
    if (bounds)
        m_defaultCell = Vec2(std::floor(bounds->center.x), std::floor(bounds->center.y));
    else
        std::cerr << "SpawnPoint: DefaultAnchor '" << m_defaultAnchor << "' is not a character drawn in the level's picture" << std::endl;

    return true;
}

Vec2 SpawnPoint::GetSpawnCell(const std::optional<Vec2> &exitCell) const
{
    // Cells are addressed by their centers, half a cell inside the region's edges.
    Vec2 halfSize = GetGridSize() * 0.5f;
    Vec2 halfCell(0.5f, 0.5f);
    Vec2 firstCell = GetGridPosition() - halfSize + halfCell;
    Vec2 lastCell = GetGridPosition() + halfSize - halfCell;

    if (!exitCell)
    {
        Vec2 bottomMiddleCell(std::floor(GetGridPosition().x), lastCell.y);
        return m_defaultCell.value_or(bottomMiddleCell);
    }

    return Vec2(std::clamp(exitCell->x, firstCell.x, lastCell.x),
                std::clamp(exitCell->y, firstCell.y, lastCell.y));
}
