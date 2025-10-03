#pragma once

#include "../Math/Vec2.h"
#include "../Graphics/Color.h"
#include "../Entity/Entity.h"
#include <cmath>

namespace Engine
{
    // Grid coordinate system - simple grid entity with configurable dimensions
    class Grid : public Entity
    {
    private:
        Vec2 m_cellCount; // Number of cells (width x height)
        float m_cellSize; // Size of each cell in world units (assuming square cells)
        Vec2 m_topLeft;   // Cached top-left position for conversions

    public:
        Grid(const Vec2 &position, const Color &color, float cellSize, const Vec2 &cellCount)
            : Entity(position, Vec2(1, 1), color), m_cellSize(cellSize), m_cellCount(cellCount) {}

        virtual void Initialize() override;

        virtual void Render() override;

        Vec2 GridToWorld(Vec2 gridPos) const;

        // Convert world position to grid coordinates
        Vec2 WorldToGrid(Vec2 worldPos) const;

        // Check if grid position is within bounds
        bool IsInBounds(Vec2 gridPos) const;

        // Getters
        Vec2 GetCellCount() const { return m_cellCount; }
        float GetCellSize() const { return m_cellSize; }
        Vec2 GetCellFromGridPosition(const Vec2 &gridPos) const { return Vec2{std::floor(gridPos.x), std::floor(gridPos.y)}; }
    };
}