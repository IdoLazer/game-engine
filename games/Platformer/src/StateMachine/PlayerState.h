#pragma once

#include <Engine.h>

// --- Forward Declarations ---
class Player;
class PlayerStateMachine;

// --- Types ---
enum class PlayerStateId
{
    Grounded,
    OnWall,
    Airborne, // parent of Jumping, Falling and Gliding
    Jumping,
    WallJumping, // child of Jumping
    Falling,
    Gliding
};

// What the player is touching after this frame's movement was resolved.
struct PlayerContacts
{
    bool ground{false};
    int wall{0}; // -1 = wall on left, 1 = wall on right, 0 = none
};

// One mode of Player's movement. A state owns its own timers and flags, decides
// its own transitions, and clears only itself on exit.
// Local to Platformer for now - see FUTURE.md for the planned promotion
// to a generic Engine::State/StateMachine once this proves itself.
class PlayerState
{
public:
    PlayerState(Player &player, PlayerStateMachine &machine)
        : m_player(player), m_machine(machine) {}
    virtual ~PlayerState() = default;

    // --- Lifecycle ---
    virtual void Enter() {}
    virtual void Exit() {}
    virtual void Update(float deltaTime) {}

    // --- Input ---
    // Return true to consume the input, false to let it bubble up to the parent state.
    virtual bool OnJumpPressed() { return false; }
    virtual bool OnJumpReleased() { return false; }
    virtual bool OnGlidePressed() { return false; }
    virtual bool OnGlideReleased() { return false; }
    virtual bool OnDirectionChanged(const Engine::Vec2 &direction) { return false; }

    // --- Collision ---
    virtual bool OnContacts(const PlayerContacts &contacts) { return false; }

    // --- Rendering ---
    virtual void AdjustVisual(Engine::Vec2 &center, Engine::Vec2 &size) const {}

    // --- Accessors ---
    virtual const char *GetName() const = 0;
    PlayerState *GetParent() const { return m_parent; }
    void SetParent(PlayerState *parent) { m_parent = parent; }

protected:
    // --- Fields ---
    Player &m_player;
    PlayerStateMachine &m_machine;

private:
    PlayerState *m_parent{nullptr};
};
