#include "Rect.h"

namespace Engine
{
    // --- Queries ---

    bool Rect::Contains(const Vec2 &point) const
    {
        Vec2 min = Min();
        Vec2 max = Max();
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y;
    }

    bool Rect::Overlaps(const Rect &other) const
    {
        Vec2 min = Min();
        Vec2 max = Max();
        Vec2 otherMin = other.Min();
        Vec2 otherMax = other.Max();
        return min.x <= otherMax.x && max.x >= otherMin.x &&
               min.y <= otherMax.y && max.y >= otherMin.y;
    }

    Rect Rect::Expanded(const Vec2 &amount) const
    {
        return Rect(center, halfExtents + amount);
    }
}
