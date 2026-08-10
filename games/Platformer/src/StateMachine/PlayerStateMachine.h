#pragma once

#include "PlayerState.h"
#include <memory>
#include <unordered_map>

// Drives a single active PlayerState: calls Exit/Enter on transitions and
// forwards per-frame Update to whichever state is current. States are
// registered once (e.g. in Initialize()) and live for Player's lifetime -
// TransitionTo only ever swaps the active pointer, no per-transition
// allocation.
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
    void RegisterState(PlayerStateId id, std::unique_ptr<PlayerState> state);

    // --- Control ---
    // Safe to call reentrantly (e.g. a buffered command executed from within
    // Enter() transitioning again) - current state is updated before Enter()
    // runs, so the innermost transition wins.
    void TransitionTo(PlayerStateId id);
    void Update(float deltaTime);

    // --- Accessors ---
    PlayerStateId GetCurrentId() const;
    bool Is(PlayerStateId id) const;

private:
    // --- Fields ---
    std::unordered_map<PlayerStateId, std::unique_ptr<PlayerState>> m_states;
    PlayerState *m_current{nullptr};
    PlayerStateId m_currentId{};
};
