#include "TileMap.h"

namespace Engine
{

// --- Type Registration ---

BEGIN_TYPE_REGISTER(TileMap)
    REGISTER_PROPERTY(const GridCoordinateSystem*, CoordSystem, &TileMap::m_coordSystem)
END_TYPE_REGISTER()

// --- Lifecycle ---

void TileMap::Render() const
{
    if (!m_coordSystem)
        return;

    Vec2 cellCount = m_coordSystem->GetCellCount();
    float cellSize = m_coordSystem->GetCellSize();
    Vec2 worldSize{cellSize, cellSize};

    for (int y = 0; y < static_cast<int>(cellCount.y); ++y)
    {
        for (int x = 0; x < static_cast<int>(cellCount.x); ++x)
        {
            Vec2 worldPos = m_coordSystem->GridToWorld(Vec2{static_cast<float>(x), static_cast<float>(y)});
            RenderTile(x, y, worldPos, worldSize);
        }
    }
}

// --- Accessors ---

void TileMap::SetCoordSystem(const GridCoordinateSystem *cs) { m_coordSystem = cs; }

}
