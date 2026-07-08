#pragma once

#include "../Math/Rect.h"

namespace Engine
{
    // Result of sweeping a moving Rect against a single static Rect.
    struct SweepHit
    {
        bool hit = false;
        float t = 1.0f;  // Fraction of `delta` traveled before contact, in [0,1]. 1.0 = full move, no contact.
        Vec2 normal{};   // Axis-aligned surface normal at the contact point (one component +/-1, other 0). Zero if !hit.
    };

    // Exact time-of-impact between a moving Rect (halfExtents may be zero, i.e. a ray/point)
    // and one static Rect, over the motion described by `delta`. Pure geometry: operates only
    // on Rect and Vec2, with no notion of grids, tiles, or entities.
    SweepHit SweepRectVsRect(const Rect &movingRect, const Vec2 &delta, const Rect &staticRect);
}
