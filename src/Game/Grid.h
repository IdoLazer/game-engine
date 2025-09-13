#pragma once

#include "Engine.h"

using namespace Engine;

// Grid coordinate system - simple grid entity with configurable dimensions
class Grid
{
private:
    Vec2 m_position;  // World position of grid center
    Vec2 m_size;      // Total size of grid in world units
    Vec2 m_cellCount; // Number of cells (width x height)
    float m_cellSize; // Size of each cell in world units (assuming square cells)
    Vec2 m_topLeft;   // Cached top-left position for conversions

public:
    Grid() = default;

    Grid(Vec2 position, float cellSize, Vec2 cellCount)
        : m_position(position), m_cellSize(cellSize), m_cellCount(cellCount)
    {
        // Calculate total world size from cell size and count
        m_size = Vec2(m_cellSize * m_cellCount.x, m_cellSize * m_cellCount.y);
        m_topLeft = Vec2(m_position.x - m_size.x * 0.5f, m_position.y + m_size.y * 0.5f);
    }

    void Initialize(Vec2 position, float cellSize, Vec2 cellCount);

    Vec2 GridToWorld(Vec2 gridPos) const;

    // Convert world position to grid coordinates
    Vec2 WorldToGrid(Vec2 worldPos) const;

    // Check if grid position is within bounds
    bool IsInBounds(Vec2 gridPos) const;

    // Getters
    Vec2 GetPosition() const { return m_position; }
    Vec2 GetSize() const { return m_size; }
    Vec2 GetCellCount() const { return m_cellCount; }
    float GetCellSize() const { return m_cellSize; }
};