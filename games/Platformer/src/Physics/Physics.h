#pragma once

#include "Collider.h"

#include <Engine.h>

#include <functional>
#include <vector>

// A sweep or ray hit, and the collider hit: null for a tile.
struct PhysicsHit : Engine::SweepHit
{
    const Collider *collider = nullptr;
};

// Collision for a level: its tiles, and the colliders added to it.
class Physics
{
// --- Constructors & Destructors ---
public:
    Physics() = default;
    ~Physics();
    Physics(const Physics &) = delete;
    Physics &operator=(const Physics &) = delete;

// --- Setup ---
public:
    // The static layer: `isSolid` says which cells block, and they sit on `layer`.
    void SetTiles(const Engine::GridCoordinateSystem &coords, std::function<bool(const Engine::Vec2 &cell)> isSolid, LayerMask layer);
    void Add(Collider &collider);
    void Remove(Collider &collider);

// --- Queries ---
public:
    // Against solid tiles and the colliders on a layer in `mask`, skipping `ignore`.
    PhysicsHit SweepSolid(const Engine::Rect &box, const Engine::Vec2 &delta, LayerMask mask, const Collider *ignore = nullptr) const;

    // Static contact test: true if `box` is touching solid, even flush with no overlap - unlike
    // SweepSolid, which excludes that case (see Sweep.cpp). `direction` must be normalized.
    bool TouchesSolid(const Engine::Rect &box, const Engine::Vec2 &direction, LayerMask mask, const Collider *ignore = nullptr) const;

    // The first tile or collider on a layer in `mask` along the ray. `direction` must be normalized.
    PhysicsHit Raycast(const Engine::Vec2 &origin, const Engine::Vec2 &direction, float maxDistance, LayerMask mask) const;

// --- Events ---
public:
    // Raised for a collider that a MoveAndPush leaves overlapping its pusher.
    Engine::EventSubscriber<Collider> &OnCrushed() { return m_onCrushedEvent; }

// --- Internal ---
private:
    friend class Collider;

    MoveResult MoveAndSlide(Collider &collider, const Engine::Vec2 &delta);
    void MoveAndPush(Collider &pusher, const Engine::Vec2 &delta);

    bool CollidesWithTiles(LayerMask mask) const;
    PhysicsHit SweepTiles(const Engine::Rect &box, const Engine::Vec2 &delta) const;
    PhysicsHit SweepColliders(const Engine::Rect &box, const Engine::Vec2 &delta, LayerMask mask, const Collider *ignore) const;
    PhysicsHit RaycastTiles(const Engine::Vec2 &origin, const Engine::Vec2 &direction, float maxDistance) const;

// --- Private Fields ---
private:
    const Engine::GridCoordinateSystem *m_coords{nullptr};
    std::function<bool(const Engine::Vec2 &cell)> m_isTileSolid;
    LayerMask m_tileLayer{0};
    std::vector<Collider *> m_colliders;
    const Collider *m_pusher{nullptr}; // The collider mid-MoveAndPush, which sweeps pass through.
    Engine::Event<Collider> m_onCrushedEvent;
};
