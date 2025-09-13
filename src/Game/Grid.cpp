#include "Grid.h"

void Grid::Initialize(Vec2 position, float cellSize, Vec2 cellCount)
{
    m_position = position;
    m_cellSize = cellSize;
    m_cellCount = cellCount;
    m_size = Vec2(m_cellSize * m_cellCount.x, m_cellSize * m_cellCount.y);
    m_topLeft = Vec2(m_position.x - m_size.x * 0.5f, m_position.y + m_size.y * 0.5f);
}

Vec2 Grid::GridToWorld(Vec2 gridPos) const
{
    // Grid (0,0) is top-left for game convenience
    // World Y increases upward, but grid Y increases downward
    float worldX = m_topLeft.x + (gridPos.x + 0.5f) * m_cellSize;
    float worldY = m_topLeft.y - (gridPos.y + 0.5f) * m_cellSize;
    return Vec2(worldX, worldY);
}

Vec2 Grid::WorldToGrid(Vec2 worldPos) const
{
    float gridX = (worldPos.x - m_topLeft.x) / m_cellSize;
    float gridY = (m_topLeft.y - worldPos.y) / m_cellSize;
    return Vec2(gridX, gridY);
}

bool Grid::IsInBounds(Vec2 gridPos) const
{
    return gridPos.x >= 0 && gridPos.x < m_cellCount.x &&
           gridPos.y >= 0 && gridPos.y < m_cellCount.y;
}
