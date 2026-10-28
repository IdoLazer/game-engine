#include "Physics.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

using namespace Engine;

namespace
{
    // Float precision slack for TouchesSolid - not a tunable detection distance.
    constexpr float kContactEpsilon = 1e-4f;

    // The earlier of two hits; `first` wins a tie.
    PhysicsHit Earlier(const PhysicsHit &first, const PhysicsHit &second)
    {
        return second.hit && second.t < first.t ? second : first;
    }
}

// --- Constructors & Destructors ---

Physics::~Physics()
{
    for (Collider *collider : m_colliders)
        collider->m_physics = nullptr;
}

// --- Setup ---

void Physics::SetTiles(const GridCoordinateSystem &coords, std::function<bool(const Vec2 &cell)> isSolid, LayerMask layer)
{
    m_coords = &coords;
    m_isTileSolid = std::move(isSolid);
    m_tileLayer = layer;
}

void Physics::Add(Collider &collider)
{
    if (collider.m_physics == this)
        return;

    if (collider.m_physics)
        collider.m_physics->Remove(collider);

    m_colliders.push_back(&collider);
    collider.m_physics = this;
}

void Physics::Remove(Collider &collider)
{
    if (std::erase(m_colliders, &collider) > 0)
        collider.m_physics = nullptr;
}

// --- Queries ---

PhysicsHit Physics::SweepSolid(const Rect &box, const Vec2 &delta, LayerMask mask, const Collider *ignore) const
{
    PhysicsHit tileHit = CollidesWithTiles(mask) ? SweepTiles(box, delta) : PhysicsHit{};
    return Earlier(tileHit, SweepColliders(box, delta, mask, ignore));
}

bool Physics::TouchesSolid(const Rect &box, const Vec2 &direction, LayerMask mask, const Collider *ignore) const
{
    // A hairline sweep in `direction`: SweepRectVsRect requires genuine (not just
    // boundary-touching) overlap on the axis perpendicular to `direction` to register a hit, so
    // solid on a different side of the box - e.g. a wall beside it, while probing below - can't
    // be mistaken for something directly ahead.
    return SweepSolid(box, direction * kContactEpsilon, mask, ignore).hit;
}

PhysicsHit Physics::Raycast(const Vec2 &origin, const Vec2 &direction, float maxDistance, LayerMask mask) const
{
    if (direction == Vec2::Zero)
        return {};

    PhysicsHit tileHit = CollidesWithTiles(mask) ? RaycastTiles(origin, direction, maxDistance) : PhysicsHit{};

    // A zero-size box swept along the ray, as the tile path does, so both hits' `t` compare directly.
    Rect rayPoint(origin, Vec2::Zero);
    return Earlier(tileHit, SweepColliders(rayPoint, direction * maxDistance, mask, nullptr));
}

// --- Movement ---

// Sweeps the whole delta in one pass, clipping and sliding along whatever it hits rather than
// resolving X and Y as two independent 1D sweeps - a separated sweep can let a diagonal approach
// slip into a corner before either axis, checked against the other's stale position, ever sees it
// coming.
MoveResult Physics::MoveAndSlide(Collider &collider, const Vec2 &delta)
{
    MoveResult result;
    Rect box = collider.GetRect();
    Vec2 remaining = delta;

    constexpr int kMaxSlideIterations = 4;
    for (int i = 0; i < kMaxSlideIterations && remaining != Vec2::Zero; ++i)
    {
        PhysicsHit hit = SweepSolid(box, remaining, collider.GetMask(), &collider);
        box.center += remaining * hit.t;

        if (!hit.hit)
            break;

        if (hit.normal.x != 0.0f)
            result.blockedX = true;
        if (hit.normal.y != 0.0f)
            result.blockedY = true;

        // Slide: keep only the leftover movement perpendicular to what was hit.
        Vec2 leftover = remaining * (1.0f - hit.t);
        remaining = leftover - hit.normal * leftover.Dot(hit.normal);
    }

    collider.GetOwner().SetGridPosition(box.center);
    return result;
}

// --- Internal ---

bool Physics::CollidesWithTiles(LayerMask mask) const
{
    return m_coords && (m_tileLayer & mask);
}

PhysicsHit Physics::SweepTiles(const Rect &box, const Vec2 &delta) const
{
    CellRange range = m_coords->GetSweptCellRange(box, delta);

    PhysicsHit bestHit;
    for (int y = static_cast<int>(range.minCell.y); y <= static_cast<int>(range.maxCell.y); ++y)
    {
        for (int x = static_cast<int>(range.minCell.x); x <= static_cast<int>(range.maxCell.x); ++x)
        {
            Vec2 cell(x, y);
            if (!m_isTileSolid(cell))
                continue;

            SweepHit hit = SweepRectVsRect(box, delta, m_coords->GetCellRect(cell));
            if (hit.hit && hit.t < bestHit.t)
                bestHit = PhysicsHit{hit, nullptr};
        }
    }
    return bestHit;
}

PhysicsHit Physics::SweepColliders(const Rect &box, const Vec2 &delta, LayerMask mask, const Collider *ignore) const
{
    PhysicsHit bestHit;
    for (const Collider *other : m_colliders)
    {
        if (other == ignore || !(other->GetLayer() & mask))
            continue;

        SweepHit hit = SweepRectVsRect(box, delta, other->GetRect());
        if (hit.hit && hit.t < bestHit.t)
            bestHit = PhysicsHit{hit, other};
    }
    return bestHit;
}

// DDA ray march (Amanatides-Woo style, one cell boundary at a time) to find the first solid
// tile along `direction`, then hands off to SweepRectVsRect for the exact hit point/normal -
// the DDA is only the broad phase here, deciding which single cell to test.
PhysicsHit Physics::RaycastTiles(const Vec2 &origin, const Vec2 &direction, float maxDistance) const
{
    Vec2 cell = m_coords->GetCellFromGridPosition(origin);
    if (m_isTileSolid(cell))
        return {}; // Starting inside solid ground - no valid hit

    constexpr float kInfinity = std::numeric_limits<float>::infinity();

    int colStep = direction.x > 0.0f ? 1 : (direction.x < 0.0f ? -1 : 0);
    int rowStep = direction.y > 0.0f ? 1 : (direction.y < 0.0f ? -1 : 0);

    float tPerCol = colStep == 0 ? kInfinity : 1.0f / std::abs(direction.x);
    float tPerRow = rowStep == 0 ? kInfinity : 1.0f / std::abs(direction.y);

    float tToNextCol = colStep == 0 ? kInfinity : ((cell.x + colStep * 0.5f) - origin.x) / direction.x;
    float tToNextRow = rowStep == 0 ? kInfinity : ((cell.y + rowStep * 0.5f) - origin.y) / direction.y;

    while (true)
    {
        float tNextBoundary = std::min(tToNextCol, tToNextRow);
        if (tNextBoundary > maxDistance)
            return {}; // Nothing solid within range

        if (tToNextCol < tToNextRow)
        {
            cell.x += colStep;
            tToNextCol += tPerCol;
        }
        else
        {
            cell.y += rowStep;
            tToNextRow += tPerRow;
        }

        if (!m_coords->IsInBounds(cell))
            return {};

        if (!m_isTileSolid(cell))
            continue;

        Rect rayPoint(origin, Vec2::Zero);
        return PhysicsHit{SweepRectVsRect(rayPoint, direction * maxDistance, m_coords->GetCellRect(cell)), nullptr};
    }
}
