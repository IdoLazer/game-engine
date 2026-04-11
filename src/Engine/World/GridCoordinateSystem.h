#pragma once

#include "../Math/Vec2.h"
#include <cmath>

namespace Engine
{
    // Pure coordinate math for a grid
    // Handles grid to world conversion, bounds checking, and cell lookups.
    class GridCoordinateSystem
    {
    private:
        Vec2 m_cellCount;
        float m_cellSize;
        Vec2 m_topLeft;

    public:
        GridCoordinateSystem() = default;
        GridCoordinateSystem(float cellSize, const Vec2 &cellCount, const Vec2 &worldPosition = Vec2{0, 0})
            : m_cellSize(cellSize), m_cellCount(cellCount)
        {
            float worldW = cellSize * cellCount.x;
            float worldH = cellSize * cellCount.y;
            m_topLeft = Vec2(worldPosition.x - worldW * 0.5f,
                             worldPosition.y + worldH * 0.5f);
        }

        Vec2 GridToWorld(Vec2 gridPos) const
        {
            float worldX = m_topLeft.x + (gridPos.x + 0.5f) * m_cellSize;
            float worldY = m_topLeft.y - (gridPos.y + 0.5f) * m_cellSize;
            return Vec2(worldX, worldY);
        }

        Vec2 WorldToGrid(Vec2 worldPos) const
        {
            float gridX = (worldPos.x - m_topLeft.x) / m_cellSize;
            float gridY = (m_topLeft.y - worldPos.y) / m_cellSize;
            return Vec2(gridX, gridY);
        }

        bool IsInBounds(Vec2 gridPos) const
        {
            return gridPos.x >= 0 && gridPos.x < m_cellCount.x &&
                   gridPos.y >= 0 && gridPos.y < m_cellCount.y;
        }

        Vec2 GetCellCount() const { return m_cellCount; }
        float GetCellSize() const { return m_cellSize; }
        Vec2 GetCellFromGridPosition(const Vec2 &gridPos) const
        {
            return Vec2{std::floor(gridPos.x), std::floor(gridPos.y)};
        }
    };
}