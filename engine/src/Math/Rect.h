#pragma once

#include "Vec2.h"

namespace Engine
{
    class Rect
    {
    public:
        Vec2 center{};
        Vec2 halfExtents{};

        // --- Constructors ---
        constexpr Rect() = default;
        constexpr explicit Rect(const Vec2 &center, const Vec2 &halfExtents)
            : center(center), halfExtents(halfExtents) {}

        // --- Accessors ---
        Vec2 Min() const { return center - halfExtents; }
        Vec2 Max() const { return center + halfExtents; }

        // --- Queries ---
        bool Contains(const Vec2 &point) const;
        bool Overlaps(const Rect &other) const;
        Rect Expanded(const Vec2 &amount) const;

        // --- Comparison ---
        bool operator==(const Rect &other) const = default;
    };
}
