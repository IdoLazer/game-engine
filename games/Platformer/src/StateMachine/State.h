#pragma once

// One node in a hierarchical state machine. A state owns its own timers and
// flags, decides its own transitions, and clears only itself on exit.
//
// Deliberately vocabulary-free: lifecycle and tree structure only. An owner
// extends it with the notifications that fit its entity (input handlers,
// collision callbacks, ...) and whatever context those need to act on.
//
// Generic on purpose, but game-local until a second use case proves the shape -
// see FUTURE.md for the planned move to engine/src/Patterns/State/.
template <typename TStateId>
class State
{
public:
    virtual ~State() = default;

    // --- Lifecycle ---
    virtual void Enter() {}
    virtual void Exit() {}
    virtual void Update(float deltaTime) {}

    // --- Accessors ---
    virtual const char *GetName() const = 0;
    State *GetParent() const { return m_parent; }
    void SetParent(State *parent) { m_parent = parent; }

private:
    State *m_parent{nullptr};
};
