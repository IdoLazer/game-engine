#pragma once

#include "PlayerState.h"
#include "StateMachine.h"

// Player's own notification vocabulary, on top of the generic tree/dispatch
// mechanism in StateMachine.h.
class PlayerStateMachine : public StateMachine<PlayerStateId, PlayerState>
{
public:
    // --- Input ---
    void JumpPressed()   { Dispatch([](PlayerState &state) { return state.OnJumpPressed(); }); }
    void JumpReleased()  { Dispatch([](PlayerState &state) { return state.OnJumpReleased(); }); }
    void GlidePressed()  { Dispatch([](PlayerState &state) { return state.OnGlidePressed(); }); }
    void GlideReleased() { Dispatch([](PlayerState &state) { return state.OnGlideReleased(); }); }
    void DirectionChanged(const Engine::Vec2 &direction)
    {
        Dispatch([&direction](PlayerState &state) { return state.OnDirectionChanged(direction); });
    }

    // --- Collision ---
    void ContactsResolved(const PlayerContacts &contacts)
    {
        Dispatch([&contacts](PlayerState &state) { return state.OnContacts(contacts); });
    }

    // --- Rendering ---
    void AdjustVisual(Engine::Vec2 &center, Engine::Vec2 &size) const
    {
        auto [chain, count] = GetActiveChain();
        for (int depth = 0; depth < count; ++depth)
            chain[depth]->AdjustVisual(center, size);
    }
};
