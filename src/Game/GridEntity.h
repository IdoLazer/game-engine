#pragma once

#include "Entity.h"
#include "Grid.h"

// Grid-based entity that lives on a grid coordinate system
// Maintains both grid position/size AND world position/size (auto-synced)
class GridEntity : public Entity
{
protected:
    const Grid *m_grid;  // Pointer to the grid this entity lives on
    Vec2 m_gridPosition; // Position in grid coordinates (cell indices)
    Vec2 m_gridSize;     // Size in grid coordinates (relative to cell size)

public:
    GridEntity(const Grid *grid) : m_grid(grid) {}
    GridEntity(const Grid *grid, const Vec2 &gridPos, const Vec2 &gridSize)
        : m_grid(grid), m_gridPosition(gridPos), m_gridSize(gridSize)
    {
        SyncGridToWorld();
    }
    GridEntity(const Grid *grid, const Vec2 &gridPos, const Vec2 &gridSize, const Color &color)
        : GridEntity(grid, gridPos, gridSize)
    {
        m_color = color;
    }
    virtual ~GridEntity() = default;

    // Grid property accessors
    Vec2 GetGridPosition() const { return m_gridPosition; }
    Vec2 GetGridSize() const { return m_gridSize; }
    const Grid *GetGrid() const { return m_grid; }

    // Set grid position and auto-update world position
    void SetGridPosition(const Vec2 &gridPos)
    {
        m_gridPosition = gridPos;
        SyncGridToWorld();
    }

    // Set grid size and auto-update world size
    void SetGridSize(const Vec2 &gridSize)
    {
        m_gridSize = gridSize;
        SyncGridToWorld();
    }

protected:
    // Auto-sync grid coordinates to world coordinates
    void SyncGridToWorld()
    {
        if (m_grid)
        {
            // Convert grid position to world position (center of cells)
            m_worldPosition = m_grid->GridToWorld(m_gridPosition);

            // Calculate world size based on grid size and cell size
            float cellSize = m_grid->GetCellSize();
            m_worldSize = Vec2(m_gridSize.x * cellSize, m_gridSize.y * cellSize);
        }
    }
};