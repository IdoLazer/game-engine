#pragma once

#include <Engine.h>

#include <cstdint>

// --- Forward Declarations ---
class Physics;

using LayerMask = std::uint32_t;

// Which axes a MoveAndSlide was stopped on.
struct MoveResult
{
    bool blockedX = false;
    bool blockedY = false;
};

// A box an entity collides with: its owner's grid position, and half its grid size.
class Collider
{
// --- Constructors & Destructors ---
public:
    Collider(Engine::GridEntity &owner, LayerMask layer, LayerMask mask);
    ~Collider();
    Collider(const Collider &) = delete;
    Collider &operator=(const Collider &) = delete;

// --- Accessors ---
public:
    Engine::GridEntity &GetOwner() const { return m_owner; }
    Engine::Rect GetRect() const;
    LayerMask GetLayer() const { return m_layer; }
    LayerMask GetMask() const { return m_mask; }

// --- Movement & Contact ---
public:
    // Moves the owner, blocked by solid tiles and colliders on a layer in the mask, sliding along them.
    MoveResult MoveAndSlide(const Engine::Vec2 &delta);
    // Moves the owner the full delta. Colliders whose mask includes this layer are pushed out of the
    // way, carried when standing on top, and crushed when they can't get out.
    void MoveAndPush(const Engine::Vec2 &delta);
    // True when flush against something solid in the mask, on the side `direction` points to.
    bool TouchesSolid(const Engine::Vec2 &direction) const;

// --- Private Fields ---
private:
    friend class Physics;

    Engine::GridEntity &m_owner;
    LayerMask m_layer;
    LayerMask m_mask;
    Physics *m_physics{nullptr};
};
