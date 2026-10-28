#pragma once

#include "Physics/Collider.h"

// The Platformer's collision layers.
namespace Layer
{
    constexpr LayerMask Terrain = 1u << 0;
    constexpr LayerMask Player = 1u << 1;
}
