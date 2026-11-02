#pragma once

#include "Physics/Collider.h"

// The Platformer's collision layers.
namespace Layer
{
    constexpr LayerMask Terrain = 1u << 0;
    constexpr LayerMask Player = 1u << 1;
    constexpr LayerMask Falcon = 1u << 2;
    constexpr LayerMask Interactive = 1u << 3;
}
