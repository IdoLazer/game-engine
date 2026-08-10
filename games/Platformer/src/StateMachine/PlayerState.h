#pragma once

// --- Types ---
enum class PlayerStateId
{
    Grounded,
    Airborne,
    OnWall,
    Gliding
};

// Base interface for a single mode of Player's movement behavior.
// Local to Platformer for now - see FUTURE.md for the planned promotion
// to a generic Engine::State/StateMachine once this proves itself.
class PlayerState
{
public:
    virtual ~PlayerState() = default;

    virtual void Enter() {}
    virtual void Exit() {}
    virtual void Update(float deltaTime) {}

    virtual const char *GetName() const = 0;
};
