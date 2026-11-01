#pragma once

#include <Engine.h>

#include <cstdint>
#include <vector>

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

    // A trigger blocks nothing and is moved by nothing: it reports the colliders on a layer in its mask.
    bool IsTrigger() const { return m_isTrigger; }
    void SetTrigger(bool isTrigger) { m_isTrigger = isTrigger; }

// --- Attachment ---
public:
    // An attached collider moves with what it's attached to.
    // Ignored unless both colliders are in the same Physics.
    void AttachTo(const Collider &target);
    void Detach() { m_attachedTo = nullptr; }
    const Collider *GetAttachedTo() const { return m_attachedTo; }

// --- Events ---
public:
    // Raised from Physics::Update for a collider that starts overlapping this trigger, and for one that stops.
    Engine::EventSubscriber<Collider> &OnEntered() { return m_onEnteredEvent; }
    Engine::EventSubscriber<Collider> &OnExited() { return m_onExitedEvent; }

    // Raised when what it's attached to is removed from Physics.
    Engine::EventSubscriber<> &OnDetached() { return m_onDetachedEvent; }

// --- Movement & Contact ---
public:
    // Moves the owner, blocked by solid tiles and colliders on a layer in the mask, sliding along them.
    MoveResult MoveAndSlide(const Engine::Vec2 &delta);
    // Moves the owner the full delta. Colliders whose mask includes this layer are pushed out of the
    // way, carried when standing on top, and crushed when they can't get out. Colliders attached to
    // this one move by the same delta, without being swept.
    void MoveAndPush(const Engine::Vec2 &delta);
    // True when flush against something solid in the mask, on the side `direction` points to.
    bool TouchesSolid(const Engine::Vec2 &direction) const;

// --- Private Fields ---
private:
    friend class Physics;

    Engine::GridEntity &m_owner;
    LayerMask m_layer;
    LayerMask m_mask;
    bool m_isTrigger{false};
    Physics *m_physics{nullptr};
    const Collider *m_attachedTo{nullptr};

    std::vector<const Collider *> m_overlapping; // What Physics::Update last found inside this trigger.
    bool m_inTriggerTile{false};                  // Whether Physics::Update last found this in a trigger cell.

    Engine::Event<Collider> m_onEnteredEvent;
    Engine::Event<Collider> m_onExitedEvent;
    Engine::Event<> m_onDetachedEvent;
};
