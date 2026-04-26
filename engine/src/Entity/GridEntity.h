#pragma once

#include "Entity.h"
#include "../World/Grid.h"

namespace Engine
{
    // Grid-based entity that lives on a Grid spatial index.
    // Maintains both grid position/size AND world position/size (auto-synced).
    class GridEntity : public Entity
    {
        DECLARE_TYPE(GridEntity, Entity)

    // --- Fields ---
    protected:
        Grid *m_grid{nullptr};
        Vec2 m_gridPosition{};
        Vec2 m_gridSize{1.0f, 1.0f};

    // --- Constructors & Destructors ---
    public:
        GridEntity() = default;
        virtual ~GridEntity();

    // --- Lifecycle ---
    public:
        virtual void Initialize() override;

    // --- Accessors ---
    public:
        Vec2 GetGridPosition() const;
        Vec2 GetGridCell() const;
        Vec2 GetGridSize() const;
        Grid *GetGrid() const;
        const GridCoordinateSystem &GetCoordinateSystem() const;
        void SetGrid(Grid *grid);
        virtual void SetGridPosition(const Vec2 &gridPos);
        void SetGridSize(const Vec2 &gridSize);

    // --- Internal ---
    protected:
        void SyncGridToWorld();

    private:
        bool m_registered{false};
    };
}