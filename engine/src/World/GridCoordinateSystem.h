#pragma once

#include "../Math/Vec2.h"
#include "../Math/Rect.h"

namespace Engine
{
    // Inclusive integer cell bounds.
    struct CellRange
    {
        Vec2 minCell;
        Vec2 maxCell;
    };

    // Pure coordinate math for a grid.
    // Handles grid-to-world conversion, bounds checking, and cell lookups.
    class GridCoordinateSystem
    {
    // --- Fields ---
    private:
        Vec2 m_cellCount{};
        float m_cellSize{0.0f};
        Vec2 m_topLeft{};

    // --- Constructors & Destructors ---
    public:
        GridCoordinateSystem() = default;
        GridCoordinateSystem(float cellSize, const Vec2 &cellCount, const Vec2 &worldPosition = Vec2{0, 0});

    // --- Coordinate Conversion ---
    public:
        Vec2 GridToWorld(Vec2 gridPos) const;
        Vec2 WorldToGrid(Vec2 worldPos) const;
        Vec2 GetCellFromGridPosition(const Vec2 &gridPos) const;
        bool IsInBounds(Vec2 gridPos) const;
        Rect GetCellRect(const Vec2 &cell) const;

        // Inclusive cell range spanning every cell `movingRect` could occupy at any point while
        // traveling `delta` - the union of its start and end positions, widened to whole cells.
        // A broad-phase helper: narrows which cells are worth testing with SweepRectVsRect,
        // without evaluating the sweep itself.
        CellRange GetSweptCellRange(const Rect &movingRect, const Vec2 &delta) const;

    // --- Accessors ---
    public:
        Vec2 GetCellCount() const;
        float GetCellSize() const;
    };
}