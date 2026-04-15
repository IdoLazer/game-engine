#pragma once

#include "GridEntity.h"

namespace Engine
{
    // A simple renderable grid tile — renders as a filled rect at its grid position.
    class GridTile : public GridEntity
    {
        DECLARE_TYPE(GridTile, GridEntity)

    // --- Constructors & Destructors ---
    public:
        GridTile() = default;

    // --- Lifecycle ---
    public:
        void Render() const override;
    };
}