#pragma once

#include "Collider.h"

#include <Engine.h>

#include <functional>
#include <optional>
#include <vector>

// What a tile cell does to a collider: nothing, blocks it, or reports it.
enum class TileCollision { None, Solid, Trigger };

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
    // The static layer: `collisionAt` says what each cell does. Solid cells block and sit on `layer`;
    // trigger cells report the colliders on a layer in `triggerMask` through OnTileEntered.
    void SetTiles(const Engine::GridCoordinateSystem &coords, std::function<TileCollision(const Engine::Vec2 &cell)> collisionAt,
                  LayerMask layer, LayerMask triggerMask);
    void Add(Collider &collider);
    void Remove(Collider &collider);

// --- Update ---
public:
    // Raises the trigger events for the overlaps that began or ended since the last update.
    void Update();

// --- Queries ---
public:
    // Against solid tiles and the colliders on a layer in `mask`, skipping `ignore` and triggers.
    PhysicsHit SweepSolid(const Engine::Rect &box, const Engine::Vec2 &delta, LayerMask mask, const Collider *ignore = nullptr) const;

    // Static contact test: true if `box` is touching solid, even flush with no overlap - unlike
    // SweepSolid, which excludes that case (see Sweep.cpp). `direction` must be normalized.
    bool TouchesSolid(const Engine::Rect &box, const Engine::Vec2 &direction, LayerMask mask, const Collider *ignore = nullptr) const;

    // The first solid tile or collider on a layer in `mask` along the ray, triggers included.
    // `direction` must be normalized.
    PhysicsHit Raycast(const Engine::Vec2 &origin, const Engine::Vec2 &direction, float maxDistance, LayerMask mask) const;

// --- Events ---
public:
    // Raised for a collider that a MoveAndPush leaves overlapping its pusher.
    Engine::EventSubscriber<Collider> &OnCrushed() { return m_onCrushedEvent; }

    // Raised from Update for a collider that steps into a trigger cell, with one such cell.
    Engine::EventSubscriber<Collider, Engine::Vec2> &OnTileEntered() { return m_onTileEnteredEvent; }

// --- Internal ---
private:
    friend class Collider;

    enum class Triggers { Skip, Include };

    MoveResult MoveAndSlide(Collider &collider, const Engine::Vec2 &delta);
    void MoveAndPush(Collider &pusher, const Engine::Vec2 &delta);

    bool CollidesWithTiles(LayerMask mask) const;
    bool IsTileSolid(const Engine::Vec2 &cell) const;
    PhysicsHit SweepTiles(const Engine::Rect &box, const Engine::Vec2 &delta) const;
    PhysicsHit SweepColliders(const Engine::Rect &box, const Engine::Vec2 &delta, LayerMask mask, const Collider *ignore, Triggers triggers) const;
    PhysicsHit RaycastTiles(const Engine::Vec2 &origin, const Engine::Vec2 &direction, float maxDistance) const;

    std::vector<const Collider *> FindOverlaps(const Collider &trigger) const;
    std::optional<Engine::Vec2> FindTriggerCell(const Engine::Rect &box) const;

// --- Private Fields ---
private:
    const Engine::GridCoordinateSystem *m_coords{nullptr};
    std::function<TileCollision(const Engine::Vec2 &cell)> m_tileCollisionAt;
    LayerMask m_tileLayer{0};
    LayerMask m_tileTriggerMask{0};
    std::vector<Collider *> m_colliders;
    const Collider *m_pusher{nullptr}; // The collider mid-MoveAndPush, which sweeps pass through.
    Engine::Event<Collider> m_onCrushedEvent;
    Engine::Event<Collider, Engine::Vec2> m_onTileEnteredEvent;
};
