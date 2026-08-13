#pragma once

#include "PlayerState.h"
#include <array>
#include <memory>
#include <optional>
#include <unordered_map>

// Drives a single active PlayerState. States are registered once (e.g. in
// Initialize()) and live for Player's lifetime - TransitionTo only ever swaps
// the active pointer, no per-transition allocation.
//
// A state may be registered under a parent, which shares behavior its children
// have in common: Update runs parent-first, inputs bubble child-first, and a
// transition between two children leaves their shared parent entered, so data
// the parent owns survives the switch.
class PlayerStateMachine
{
public:
    // --- Constructors & Destructors ---
    PlayerStateMachine() = default;
    ~PlayerStateMachine() = default;

    // Non-copyable since we're managing unique_ptrs
    PlayerStateMachine(const PlayerStateMachine &) = delete;
    PlayerStateMachine &operator=(const PlayerStateMachine &) = delete;

    // --- Setup ---
    // A parent must be registered before its children.
    void RegisterState(PlayerStateId id, std::unique_ptr<PlayerState> state,
                       std::optional<PlayerStateId> parent = std::nullopt);

    // --- Control ---
    void TransitionTo(PlayerStateId id);
    void Update(float deltaTime);

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
    void AdjustVisual(Engine::Vec2 &center, Engine::Vec2 &size) const;

    // --- Accessors ---
    // For debugging. Nothing outside the states should be branching on this -
    // deciding what a state means is the state's own job.
    PlayerStateId GetCurrentId() const;
    const char *GetCurrentName() const;

    // Typed access to a registered state, for the setup a caller does right
    // before transitioning into it.
    template <typename TState>
    TState &Get() { return static_cast<TState &>(*m_states.at(TState::Id)); }

private:
    // --- Internal ---
    // Offers the notification to the active state first, then up through its
    // ancestors, stopping at the first one that consumes it.
    template <typename THandler>
    void Dispatch(THandler &&handler)
    {
        for (PlayerState *state = m_current; state; state = state->GetParent())
        {
            if (handler(*state)) return;
        }
    }

    PlayerState *FindCommonAncestor(PlayerState *a, PlayerState *b) const;
    static bool IsAncestorOrSelf(const PlayerState *ancestor, PlayerState *state);

    // Root-to-leaf chain of a state and its ancestors, deepest last.
    static constexpr int kMaxDepth = 4;
    static std::array<PlayerState *, kMaxDepth> BuildChain(PlayerState *state, int &outCount);

    // --- Fields ---
    std::unordered_map<PlayerStateId, std::unique_ptr<PlayerState>> m_states;
    PlayerState *m_current{nullptr};
    PlayerStateId m_currentId{};
};
