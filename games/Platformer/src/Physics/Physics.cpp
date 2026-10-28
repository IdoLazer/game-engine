#include "Physics.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

using namespace Engine;

namespace
{
    // Float precision slack for contact and overlap tests - not a tunable detection distance.
    constexpr float kContactEpsilon = 1e-4f;

    // The earlier of two hits; `first` wins a tie.
    PhysicsHit Earlier(const PhysicsHit &first, const PhysicsHit &second)
    {
        return second.hit && second.t < first.t ? second : first;
    }

    // Boxes that only touch don't overlap; the slack absorbs the float error that would make them.
    bool Overlap(const Rect &first, const Rect &second)
    {
        Vec2 reach = first.halfExtents + second.halfExtents;
        float apartX = std::abs(first.center.x - second.center.x);
        float apartY = std::abs(first.center.y - second.center.y);
        return reach.x - apartX > kContactEpsilon && reach.y - apartY > kContactEpsilon;
    }

    // True when `box` rests on top of `platform`: a hairline sweep down hits it.
    bool IsStandingOn(const Rect &box, const Rect &platform)
    {
        return SweepRectVsRect(box, Vec2(0.0f, kContactEpsilon), platform).hit;
    }

    // Where `box`'s center ends up when `pusher`, having moved by `step`, pushes it clear: on the pusher's
    // far face grown by the box's half-size. Placed exactly there, float error can't leave it a hair
    // inside the pusher, where it would be stuck against every move along the face.
    Vec2 PushedCenter(const Rect &pusher, const Rect &box, const Vec2 &step)
    {
        Rect reach = pusher.Expanded(box.halfExtents);
        Vec2 farFace = step.x + step.y > 0.0f ? reach.Max() : reach.Min();
        return Vec2(step.x != 0.0f ? farFace.x : box.center.x,
                    step.y != 0.0f ? farFace.y : box.center.y);
    }

    // Where `box`'s center rests exactly on top of `platform`.
    Vec2 RestingCenter(const Rect &platform, const Rect &box)
    {
        return Vec2(box.center.x, platform.Expanded(box.halfExtents).Min().y);
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

// The pusher moves the whole delta, one axis at a time: what it runs into is pushed clear, and what
// rides on it is carried. Riders are found first, because moving the pusher breaks the contact.
void Physics::MoveAndPush(Collider &pusher, const Vec2 &delta)
{
    struct Candidate
    {
        Collider *collider;
        bool isRider;
    };

    std::vector<Candidate> candidates;
    for (Collider *other : m_colliders)
    {
        if (other != &pusher && (other->GetMask() & pusher.GetLayer()))
            candidates.push_back({other, IsStandingOn(other->GetRect(), pusher.GetRect())});
    }

    m_pusher = &pusher;

    for (const Vec2 &step : {Vec2(delta.x, 0.0f), Vec2(0.0f, delta.y)})
    {
        if (step == Vec2::Zero)
            continue;

        GridEntity &owner = pusher.GetOwner();
        owner.SetGridPosition(owner.GetGridPosition() + step);

        Rect pusherBox = pusher.GetRect();

        for (const Candidate &candidate : candidates)
        {
            Collider &collider = *candidate.collider;
            Rect box = collider.GetRect();

            if (Overlap(box, pusherBox))
            {
                MoveAndSlide(collider, PushedCenter(pusherBox, box, step) - box.center);

                if (Overlap(collider.GetRect(), pusherBox))
                    m_onCrushedEvent.Notify(collider);
            }
            else if (candidate.isRider)
            {
                bool movedVertically = step.y != 0.0f;
                MoveAndSlide(collider, movedVertically ? RestingCenter(pusherBox, box) - box.center : step);
            }
        }
    }

    m_pusher = nullptr;
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
        if (other == ignore || other == m_pusher || !(other->GetLayer() & mask))
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
