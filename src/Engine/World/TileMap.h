#pragma once

#include "../Entity/Entity.h"
#include "GridCoordinateSystem.h"

namespace Engine
{
    // Renderable tile map — an Entity that loops over grid cells and calls a virtual
    // RenderTile() for each one. Subclass and override RenderTile to customize appearance.
    class TileMap : public Entity
    {
    protected:
        const GridCoordinateSystem *m_coordSystem;

    public:
        TileMap(const GridCoordinateSystem *coordSystem)
            : m_coordSystem(coordSystem) {}

        virtual ~TileMap() = default;

        void Render() const override
        {
            Vec2 cellCount = m_coordSystem->GetCellCount();
            float cellSize = m_coordSystem->GetCellSize();
            Vec2 worldSize{cellSize, cellSize};

            for (int y = 0; y < static_cast<int>(cellCount.y); ++y)
            {
                for (int x = 0; x < static_cast<int>(cellCount.x); ++x)
                {
                    Vec2 worldPos = m_coordSystem->GridToWorld(Vec2{static_cast<float>(x), static_cast<float>(y)});
                    RenderTile(x, y, worldPos, worldSize);
                }
            }
        }

    protected:
        // Override in subclasses to control how each tile is drawn
        virtual void RenderTile(int x, int y, const Vec2 &worldPos, const Vec2 &worldSize) const = 0;
    };
}
