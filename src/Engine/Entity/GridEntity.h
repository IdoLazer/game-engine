#pragma once

#include "Entity.h"
#include "../World/Grid.h"

namespace Engine
{
    // Grid-based entity that lives on a Grid spatial index.
    // Maintains both grid position/size AND world position/size (auto-synced).
    class GridEntity : public Entity
    {
    protected:
        Grid *m_grid;            // The grid this entity lives on (can be nullptr)
        Vec2 m_gridPosition;     // Position in grid coordinates (cell indices)
        Vec2 m_gridSize;         // Size in grid coordinates (relative to cell size)

    public:
        GridEntity(Grid *grid) : m_grid(grid) {}
        GridEntity(Grid *grid, const Vec2 &gridPos, const Vec2 &gridSize)
            : m_grid(grid), m_gridPosition(gridPos), m_gridSize(gridSize)
        {
            SyncGridToWorld();
        }
        GridEntity(Grid *grid, const Vec2 &gridPos, const Vec2 &gridSize, const Color &color)
            : GridEntity(grid, gridPos, gridSize)
        {
            m_color = color;
        }

        virtual ~GridEntity()
        {
            if (m_grid && m_registered)
            {
                Vec2 cell = m_grid->GetCellFromGridPosition(m_gridPosition);
                m_grid->Unregister(this, cell);
            }
        }

        virtual void Initialize() override
        {
            if (m_grid)
            {
                Vec2 cell = m_grid->GetCellFromGridPosition(m_gridPosition);
                m_grid->Register(this, cell);
                m_registered = true;
            }
        }

        // Grid property accessors
        Vec2 GetGridPosition() const { return m_gridPosition; }
        Vec2 GetGridCell() const { return m_grid ? m_grid->GetCellFromGridPosition(m_gridPosition) : Vec2{}; }
        Vec2 GetGridSize() const { return m_gridSize; }
        Grid *GetGrid() const { return m_grid; }
        const GridCoordinateSystem &GetCoordinateSystem() const { return m_grid->GetCoordinateSystem(); }

        // Set grid position and auto-update world position + grid registration
        virtual void SetGridPosition(const Vec2 &gridPos)
        {
            if (m_grid && m_registered)
            {
                Vec2 oldCell = m_grid->GetCellFromGridPosition(m_gridPosition);
                Vec2 newCell = m_grid->GetCellFromGridPosition(gridPos);
                if (!(oldCell == newCell))
                    m_grid->UpdateRegistration(this, oldCell, newCell);
            }
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
                const auto &coordSystem = m_grid->GetCoordinateSystem();
                m_worldPosition = m_grid->GridToWorld(m_gridPosition);
                float cellSize = m_grid->GetCellSize();
                m_worldSize = Vec2(m_gridSize.x * cellSize, m_gridSize.y * cellSize);
            }
        }

    private:
        bool m_registered{false}; // Whether this entity is registered in the Grid's spatial index
    };
}