#include "Sweep.h"
#include <algorithm>
#include <limits>

namespace Engine
{
    namespace
    {
        constexpr float kInfinity = std::numeric_limits<float>::infinity();

        // The "slab" test, on a single axis: given a point at `center` moving by `delta` over
        // t in (-inf, inf), find the interval [entry, exit] during which its coordinate on this
        // axis lies within [minBound, maxBound]. A finite `delta` crosses each boundary once,
        // at the t that solves center + t*delta = bound; entry is the earlier crossing, exit
        // the later one.
        //
        // A zero `delta` never crosses a boundary, so this axis instead reports one of two fixed
        // states: unconstrained (entry = -inf, exit = +inf) if `center` is strictly between the
        // bounds, meaning this axis never restricts which t are valid and the other axis's own
        // interval decides the result alone; or unsatisfiable (entry = +inf, exit = -inf)
        // otherwise, meaning no t satisfies this axis and the combined test is a miss regardless
        // of the other axis. The choice between them is a strict inequality, so a point sitting
        // exactly on the boundary counts as unsatisfiable rather than inside - touching a bound
        // without crossing it, on an axis with no motion, must not register as "in range" on
        // that axis, or two rects merely resting flush against each other (touching, not
        // overlapping) would report a hit against any motion at all along the other axis.
        void SweepAxis(float center, float delta, float minBound, float maxBound, float &entry, float &exit)
        {
            if (delta == 0.0f)
            {
                if (center > minBound && center < maxBound)
                {
                    entry = -kInfinity;
                    exit = kInfinity;
                }
                else
                {
                    entry = kInfinity;
                    exit = -kInfinity;
                }
                return;
            }

            float t1 = (minBound - center) / delta;
            float t2 = (maxBound - center) / delta;
            entry = std::min(t1, t2);
            exit = std::max(t1, t2);
        }
    }

    SweepHit SweepRectVsRect(const Rect &movingRect, const Vec2 &delta, const Rect &staticRect)
    {
        // Minkowski-sum trick: expand the static rect by the mover's half-extents, then sweep
        // the mover's center (a point) against it - a moving box vs a moving point are the same test.
        Rect expanded = staticRect.Expanded(movingRect.halfExtents);
        Vec2 min = expanded.Min();
        Vec2 max = expanded.Max();

        float entryX, exitX, entryY, exitY;
        SweepAxis(movingRect.center.x, delta.x, min.x, max.x, entryX, exitX);
        SweepAxis(movingRect.center.y, delta.y, min.y, max.y, entryY, exitY);

        // The point is inside the 2D box exactly when it's inside both axes' slabs at once, i.e.
        // during the intersection of their two intervals: [max(entryX, entryY), min(exitX, exitY)].
        float tEntry = std::max(entryX, entryY);
        float tExit = std::min(exitX, exitY);

        // tEntry > tExit: the two intervals don't overlap at all - never touching.
        // tExit <= 0 (not just < 0): the overlap interval lies entirely at or before the start
        // of the sweep. This also covers a mover that already touches the target at t = 0 and is
        // heading away from it - its interval ends at exactly tExit == 0, so there's no
        // strictly-positive instant of forward motion during which it's actually inside. An
        // object already in contact must stay free to move away without that contact being
        // reported as a hit.
        // tEntry > 1: the earliest possible contact happens after this sweep's motion ends.
        SweepHit result;
        if (tEntry > tExit || tExit <= 0.0f || tEntry > 1.0f)
        {
            return result; // hit = false, t = 1.0, normal = zero
        }

        result.hit = true;
        result.t = std::clamp(tEntry, 0.0f, 1.0f);

        // The blocking axis is whichever one produced tEntry; the normal points back the way we came.
        if (entryX > entryY)
        {
            result.normal = Vec2(delta.x > 0.0f ? -1.0f : 1.0f, 0.0f);
        }
        else
        {
            result.normal = Vec2(0.0f, delta.y > 0.0f ? -1.0f : 1.0f);
        }
        return result;
    }
}
