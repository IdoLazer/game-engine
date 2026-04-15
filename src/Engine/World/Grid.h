#pragma once

#include "../Math/Vec2.h"
#include "GridCoordinateSystem.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <type_traits>

namespace Engine
{
    class GridEntity;

    // Grid spatial index
    // Owns a GridCoordinateSystem for coordinate math and tracks GridEntities by cell.
    class Grid
    {
    public:
        // --- Constructors & Destructors ---
        Grid() = default;
        Grid(float cellSize, const Vec2 &cellCount, const Vec2 &worldPosition = Vec2{0, 0});

        // --- Registration (called by GridEntity lifecycle) ---
        void Register(GridEntity *entity, const Vec2 &cell);
        void Unregister(GridEntity *entity, const Vec2 &cell);
        void UpdateRegistration(GridEntity *entity, const Vec2 &oldCell, const Vec2 &newCell);

        // --- Spatial Queries ---
        bool IsOccupied(const Vec2 &cell) const;

        // Returns all entities at a cell that match type T
        template <typename T>
        std::vector<T *> GetEntitiesAt(const Vec2 &cell) const
        {
            static_assert(std::is_base_of_v<GridEntity, T>, "T must derive from GridEntity");
            std::vector<T *> result;
            int key = CellKey(cell);
            auto it = m_entityMap.find(key);
            if (it != m_entityMap.end())
            {
                for (GridEntity *e : it->second)
                {
                    T *cast = dynamic_cast<T *>(e);
                    if (cast)
                        result.push_back(cast);
                }
            }
            return result;
        }

        // Returns the first entity at a cell that matches type T, or nullptr
        template <typename T>
        T *GetFirstEntityAt(const Vec2 &cell) const
        {
            static_assert(std::is_base_of_v<GridEntity, T>, "T must derive from GridEntity");
            int key = CellKey(cell);
            auto it = m_entityMap.find(key);
            if (it != m_entityMap.end())
            {
                for (GridEntity *e : it->second)
                {
                    T *cast = dynamic_cast<T *>(e);
                    if (cast)
                        return cast;
                }
            }
            return nullptr;
        }

        // --- Coordinate Math (delegated) ---
        Vec2 GridToWorld(Vec2 gridPos) const;
        Vec2 WorldToGrid(Vec2 worldPos) const;
        bool IsInBounds(Vec2 gridPos) const;

        // --- Accessors ---
        Vec2 GetCellCount() const;
        float GetCellSize() const;
        Vec2 GetCellFromGridPosition(const Vec2 &gridPos) const;
        const GridCoordinateSystem &GetCoordinateSystem() const;

    private:
        // --- Fields ---
        GridCoordinateSystem m_coordSystem;
        std::unordered_map<int, std::vector<GridEntity *>> m_entityMap;

        // --- Internal ---
        int CellKey(const Vec2 &cell) const
        {
            return static_cast<int>(cell.y) * static_cast<int>(m_coordSystem.GetCellCount().x) + static_cast<int>(cell.x);
        }
    };
}