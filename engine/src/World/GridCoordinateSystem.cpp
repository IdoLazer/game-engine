#include "GridCoordinateSystem.h"
#include <cmath>

namespace Engine
{

// --- Constructors ---

GridCoordinateSystem::GridCoordinateSystem(float cellSize, const Vec2 &cellCount, const Vec2 &worldPosition)
    : m_cellSize(cellSize), m_cellCount(cellCount)
{
    float worldW = cellSize * cellCount.x;
    float worldH = cellSize * cellCount.y;
    m_topLeft = Vec2(worldPosition.x - worldW * 0.5f,
                     worldPosition.y + worldH * 0.5f);
}

// --- Coordinate Conversion ---

Vec2 GridCoordinateSystem::GridToWorld(Vec2 gridPos) const
{
    float worldX = m_topLeft.x + (gridPos.x + 0.5f) * m_cellSize;
    float worldY = m_topLeft.y - (gridPos.y + 0.5f) * m_cellSize;
    return Vec2(worldX, worldY);
}

Vec2 GridCoordinateSystem::WorldToGrid(Vec2 worldPos) const
{
    float gridX = (worldPos.x - m_topLeft.x) / m_cellSize - 0.5f;
    float gridY = (m_topLeft.y - worldPos.y) / m_cellSize - 0.5f;
    return Vec2(gridX, gridY);
}

Vec2 GridCoordinateSystem::GetCellFromGridPosition(const Vec2 &gridPos) const
{
    return Vec2{std::floor(gridPos.x + 0.5f), std::floor(gridPos.y + 0.5f)};
}

bool GridCoordinateSystem::IsInBounds(Vec2 gridPos) const
{
    return gridPos.x >= -0.5f && gridPos.x < m_cellCount.x - 0.5f &&
           gridPos.y >= -0.5f && gridPos.y < m_cellCount.y - 0.5f;
}

// --- Accessors ---

Vec2 GridCoordinateSystem::GetCellCount() const { return m_cellCount; }
float GridCoordinateSystem::GetCellSize() const { return m_cellSize; }

}
