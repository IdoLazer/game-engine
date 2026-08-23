#pragma once

#include "State.h"
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
// Adds Player's own notification vocabulary (input, collision, visual
// adjustment) on top of the generic tree/lifecycle mechanism in State.h.
class PlayerState : public State<PlayerStateId>
{
public:
    PlayerState(Player &player, PlayerStateMachine &machine)
        : m_player(player), m_machine(machine) {}

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

protected:
    // --- Fields ---
    Player &m_player;
    PlayerStateMachine &m_machine;
};
