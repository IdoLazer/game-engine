#include "Grid.h"
#include "../Entity/GridEntity.h"

namespace Engine
{
    // --- Constructors & Destructors ---

    Grid::Grid(float cellSize, const Vec2 &cellCount, const Vec2 &worldPosition)
        : m_coordSystem(cellSize, cellCount, worldPosition) {}

    // --- Registration ---

    void Grid::Register(GridEntity *entity, const Vec2 &cell)
    {
        int key = CellKey(cell);
        m_entityMap[key].push_back(entity);
    }

    void Grid::Unregister(GridEntity *entity, const Vec2 &cell)
    {
        int key = CellKey(cell);
        auto it = m_entityMap.find(key);
        if (it != m_entityMap.end())
        {
            auto &vec = it->second;
            std::erase(vec, entity);
            if (vec.empty())
                m_entityMap.erase(it);
        }
    }

    void Grid::UpdateRegistration(GridEntity *entity, const Vec2 &oldCell, const Vec2 &newCell)
    {
        Unregister(entity, oldCell);
        Register(entity, newCell);
    }

    // --- Spatial Queries ---

    bool Grid::IsOccupied(const Vec2 &cell) const
    {
        int key = CellKey(cell);
        auto it = m_entityMap.find(key);
        return it != m_entityMap.end() && !it->second.empty();
    }

    // --- Coordinate Math (delegated) ---

    Vec2 Grid::GridToWorld(Vec2 gridPos) const { return m_coordSystem.GridToWorld(gridPos); }
    Vec2 Grid::WorldToGrid(Vec2 worldPos) const { return m_coordSystem.WorldToGrid(worldPos); }
    bool Grid::IsInBounds(Vec2 gridPos) const { return m_coordSystem.IsInBounds(gridPos); }

    // --- Accessors ---

    Vec2 Grid::GetCellCount() const { return m_coordSystem.GetCellCount(); }
    float Grid::GetCellSize() const { return m_coordSystem.GetCellSize(); }
    Vec2 Grid::GetCellFromGridPosition(const Vec2 &gridPos) const { return m_coordSystem.GetCellFromGridPosition(gridPos); }
    const GridCoordinateSystem &Grid::GetCoordinateSystem() const { return m_coordSystem; }
}