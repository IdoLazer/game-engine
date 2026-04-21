#pragma once

#include "Entity.h"

namespace Engine
{
    // A simple renderable tile — renders as a filled rect at its world position.
    class Tile : public Entity
    {
        DECLARE_TYPE(Tile, Entity)

    // --- Lifecycle ---
    public:
        void Render() const override;
    };
}
