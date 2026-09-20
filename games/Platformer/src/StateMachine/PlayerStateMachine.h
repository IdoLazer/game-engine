#pragma once

#include "PlayerState.h"
#include "StateMachine.h"

// Player's own notification vocabulary, on top of the generic tree/dispatch
// mechanism in StateMachine.h.
class PlayerStateMachine : public StateMachine<PlayerStateId, PlayerState>
{
public:
    // --- Input ---
    // Each returns whether a state acted on the input - see Player::Jump.
    bool JumpPressed()   { return Dispatch([](PlayerState &state) { return state.OnJumpPressed(); }); }
    bool JumpReleased()  { return Dispatch([](PlayerState &state) { return state.OnJumpReleased(); }); }
    bool GlidePressed()  { return Dispatch([](PlayerState &state) { return state.OnGlidePressed(); }); }
    bool GlideReleased() { return Dispatch([](PlayerState &state) { return state.OnGlideReleased(); }); }
    bool DirectionChanged(const Engine::Vec2 &direction)
    {
        return Dispatch([&direction](PlayerState &state) { return state.OnDirectionChanged(direction); });
    }

    // --- Collision ---
    bool ContactsResolved(const PlayerContacts &contacts)
    {
        return Dispatch([&contacts](PlayerState &state) { return state.OnContacts(contacts); });
    }

    // --- Rendering ---
    void Render(Engine::Vec2 &center, Engine::Vec2 &size) const
    {
        if (const PlayerState *state = GetCurrentState()) state->Render(center, size);
    }
};
