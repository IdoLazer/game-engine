#pragma once

#include "../Entity/Entity.h"
#include "GridCoordinateSystem.h"

namespace Engine
{
    // Renderable tile map — an Entity that loops over grid cells and calls a virtual
    // RenderTile() for each one. Subclass and override RenderTile to customize appearance.
    class TileMap : public Entity
    {
        DECLARE_TYPE(TileMap, Entity)

    // --- Fields ---
    protected:
        const GridCoordinateSystem *m_coordSystem{nullptr};

    // --- Constructors & Destructors ---
    public:
        TileMap() = default;
        virtual ~TileMap() = default;

    // --- Lifecycle ---
    public:
        void Render() const override;

    // --- Accessors ---
    public:
        void SetCoordSystem(const GridCoordinateSystem *cs);

    // --- Subclass Interface ---
    protected:
        virtual void RenderTile(int x, int y, const Vec2 &worldPos, const Vec2 &worldSize) const = 0;
    };
}
