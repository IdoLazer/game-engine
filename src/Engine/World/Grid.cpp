#include "Grid.h"
#include "../Rendering/Renderer2D.h"

namespace Engine
{
    void Grid::Initialize()
    {
        // Calculate total world size from cell size and count
        m_worldSize = Vec2(m_cellSize * m_cellCount.x, m_cellSize * m_cellCount.y);
        m_topLeft = Vec2(m_worldPosition.x - m_worldSize.x * 0.5f, m_worldPosition.y + m_worldSize.y * 0.5f);
    }

    void Grid::Render()
    {
        Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);
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
}