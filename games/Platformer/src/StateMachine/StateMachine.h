#pragma once

#include "State.h"
#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <utility>

// Drives a single active TState. States are registered once (e.g. in an
// Initialize()) and live for the machine's lifetime - TransitionTo only ever
// swaps the active pointer, no per-transition allocation.
//
// A state may be registered under a parent, which shares behavior its children
// have in common: Update runs parent-first, and a transition between two
// children leaves their shared parent entered, so data the parent owns survives
// the switch.
//
// Deliberately vocabulary-free: this carries the tree and the dispatch
// mechanism, nothing else. An owner subclasses it and adds its own typed
// notifications on top of the protected Dispatch() and GetActiveChain().
//
// TState is the common base every registered state derives from. Naming it as a
// parameter, rather than always working through State<TStateId>, is what lets a
// subclass's notifications take their own state type directly instead of
// casting down at every call site.
//
// Generic on purpose, but game-local until a second use case proves the shape -
// see FUTURE.md for the planned move to engine/src/Patterns/State/.
template <typename TStateId, typename TState = State<TStateId>>
class StateMachine
{
    static_assert(std::is_base_of_v<State<TStateId>, TState>,
                  "TState must derive from State<TStateId>");

    // Deepest root-to-leaf chain supported. Must stay declared above the
    // members whose signatures reference it.
    static constexpr int kMaxDepth = 4;

public:
    // --- Constructors & Destructors ---
    StateMachine() = default;
    virtual ~StateMachine() = default;

    // Non-copyable since we're managing unique_ptrs
    StateMachine(const StateMachine &) = delete;
    StateMachine &operator=(const StateMachine &) = delete;

    // --- Setup ---
    // A parent must be registered before its children.
    void RegisterState(TStateId id, std::unique_ptr<TState> state,
                       std::optional<TStateId> parent = std::nullopt)
    {
        if (parent)
            state->SetParent(m_states.at(*parent).get());

        m_states[id] = std::move(state);
    }

    // --- Control ---
    void TransitionTo(TStateId id)
    {
        if (m_current && m_currentId == id) return;

        TState *target = m_states.at(id).get();
        TState *shared = FindCommonAncestor(m_current, target);

        // Everything from the shared ancestor upwards stays entered.
        for (TState *state = m_current; state != shared; state = ParentOf(state))
            state->Exit();

        // Set current before Enter() runs so a reentrant TransitionTo sees
        // consistent state and its result sticks once this call unwinds.
        m_currentId = id;
        m_current = target;

        auto [chain, count] = BuildChain(target);

        int depth = 0;
        if (shared)
        {
            while (depth < count && chain[depth] != shared) ++depth;
            ++depth; // start below the shared ancestor
        }

        for (; depth < count; ++depth)
        {
            if (m_current != target) return; // a nested transition took over
            chain[depth]->Enter();
        }
    }

    void Update(float deltaTime)
    {
        TState *active = m_current;
        auto [chain, count] = GetActiveChain();

        for (int depth = 0; depth < count; ++depth)
        {
            if (m_current != active) return; // transitioned mid-update; the new state starts next frame
            chain[depth]->Update(deltaTime);
        }
    }

    // --- Accessors ---
    // For debugging. Nothing outside the states should be branching on this -
    // deciding what a state means is the state's own job.
    TStateId GetCurrentId() const { return m_currentId; }
    const char *GetCurrentName() const { return m_current ? m_current->GetName() : "None"; }

    // Typed access to a registered state, for the setup a caller does right
    // before transitioning into it.
    template <typename TConcrete>
    TConcrete &Get() { return static_cast<TConcrete &>(*m_states.at(TConcrete::Id)); }

protected:
    // Offers the notification to the active state first, then up through its
    // ancestors, stopping at the first one that consumes it. THandler takes a
    // TState& and returns true once the notification is consumed.
    template <typename THandler>
    void Dispatch(THandler &&handler)
    {
        for (TState *state = m_current; state; state = ParentOf(state))
        {
            if (handler(*state)) return;
        }
    }

    // Root-to-leaf chain of the active state and its ancestors, deepest last,
    // for notifications every level gets a say in rather than the first
    // consumer only.
    std::pair<std::array<TState *, kMaxDepth>, int> GetActiveChain() const
    {
        return BuildChain(m_current);
    }

private:
    // --- Internal ---
    static TState *ParentOf(TState *state)
    {
        return state ? static_cast<TState *>(state->GetParent()) : nullptr;
    }

    TState *FindCommonAncestor(TState *a, TState *b) const
    {
        for (TState *state = a; state; state = ParentOf(state))
        {
            if (IsAncestorOrSelf(state, b)) return state;
        }
        return nullptr;
    }

    static bool IsAncestorOrSelf(const TState *ancestor, TState *state)
    {
        for (TState *current = state; current; current = ParentOf(current))
        {
            if (current == ancestor) return true;
        }
        return false;
    }

    static std::pair<std::array<TState *, kMaxDepth>, int> BuildChain(TState *state)
    {
        std::array<TState *, kMaxDepth> chain{};
        int count = 0;

        for (TState *current = state; current && count < kMaxDepth; current = ParentOf(current))
            chain[count++] = current;

        std::reverse(chain.begin(), chain.begin() + count);
        return {chain, count};
    }

    // --- Fields ---
    std::unordered_map<TStateId, std::unique_ptr<TState>> m_states;
    TState *m_current{nullptr};
    TStateId m_currentId{};
};
