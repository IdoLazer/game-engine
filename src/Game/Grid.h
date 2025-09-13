#pragma once

#include "Engine.h"

using namespace Engine;

// Grid coordinate system - simple grid entity with configurable dimensions
class Grid
{
private:
    Vec2 m_position;  // World position of grid center
    Vec2 m_size;      // Total size of grid in world units
    Color m_color;    // Color of the grid
    Vec2 m_cellCount; // Number of cells (width x height)
    float m_cellSize; // Size of each cell in world units (assuming square cells)
    Vec2 m_topLeft;   // Cached top-left position for conversions

public:
    Grid() = default;

    void Initialize(Vec2 position, float cellSize, Vec2 cellCount, Color color);

    void Render() const;

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