#include "SpawnPoint.h"

#include <algorithm>
#include <cmath>

using namespace Engine;

// --- Type Registration ---

BEGIN_TYPE_REGISTER(SpawnPoint)
    REGISTER_PROPERTY(std::string, Name, &SpawnPoint::m_name)
END_TYPE_REGISTER()

// --- Public Interface ---

Vec2 SpawnPoint::GetSpawnCell(const std::optional<Vec2> &exitCell) const
{
    Vec2 reach = (GetGridSize() - Vec2(1.0f, 1.0f)) * 0.5f; // From the middle to the outermost cells
    Vec2 firstCell = GetGridPosition() - reach;
    Vec2 lastCell = GetGridPosition() + reach;

    if (!exitCell)
        return Vec2(std::floor(GetGridPosition().x), lastCell.y);

    return Vec2(std::clamp(exitCell->x, firstCell.x, lastCell.x),
                std::clamp(exitCell->y, firstCell.y, lastCell.y));
}
