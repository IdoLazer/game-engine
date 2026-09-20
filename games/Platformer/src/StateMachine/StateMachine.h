#pragma once

#include "State.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <utility>

// Drives a single active TState. States are registered once and live for the
// machine's lifetime - TransitionTo only swaps the active pointer.
//
// Update runs parent-first, and a transition between two children leaves their
// shared parent entered, so data the parent owns survives the switch.
//
// Vocabulary-free: an owner subclasses it and adds its own typed notifications
// on top of Dispatch(). TState is that owner's state base, so those
// notifications take it directly instead of casting down.
//
// Game-local for now - see FUTURE.md for the move to engine/src/Patterns/State/.
template <typename TStateId, typename TState = State<TStateId>>
class StateMachine
{
    static_assert(std::is_base_of_v<State<TStateId>, TState>,
                  "TState must derive from State<TStateId>");

    // Must stay above the members whose signatures reference it.
    static constexpr int kMaxDepth = 4;

public:
    // --- Constructors & Destructors ---
    StateMachine() = default;
    virtual ~StateMachine() = default;

    // Non-copyable since we're managing unique_ptrs
    StateMachine(const StateMachine &) = delete;
    StateMachine &operator=(const StateMachine &) = delete;

    // --- Setup ---
    // A parent must be registered before its children. A state declaring
    // `static constexpr TStateId RequiredAncestor` must be registered under it;
    // misplacing one fails silently otherwise, since Get<> still reaches an
    // ancestor that never runs.
    template <typename TConcrete>
    void RegisterState(TStateId id, std::unique_ptr<TConcrete> state,
                       std::optional<TStateId> parent = std::nullopt)
    {
        static_assert(std::is_base_of_v<TState, TConcrete>,
                      "A registered state must derive from TState");

        if (parent)
            state->SetParent(m_states.at(*parent).get());

        TState *registered = state.get();
        m_states[id] = std::move(state);

        if constexpr (requires { TConcrete::RequiredAncestor; })
        {
            auto ancestor = m_states.find(TConcrete::RequiredAncestor);
            if (ancestor == m_states.end() || !IsAncestorOrSelf(ancestor->second.get(), registered))
            {
                std::cerr << "StateMachine: " << registered->GetName()
                          << " is written to run under an ancestor it was not registered below."
                          << std::endl;
            }
        }
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
    // For debugging - deciding what a state means is the state's own job.
    TStateId GetCurrentId() const { return m_currentId; }
    const char *GetCurrentName() const { return m_current ? m_current->GetName() : "None"; }

    // For the setup a caller does right before transitioning in.
    template <typename TConcrete>
    TConcrete &Get() { return static_cast<TConcrete &>(*m_states.at(TConcrete::Id)); }

protected:
    // Active state first, then up through its ancestors. Returns whether any consumed it.
    template <typename THandler>
    bool Dispatch(THandler &&handler)
    {
        for (TState *state = m_current; state; state = ParentOf(state))
        {
            if (handler(*state)) return true;
        }
        return false;
    }

    // Root-to-leaf, deepest last - for notifications every level gets a say in.
    std::pair<std::array<TState *, kMaxDepth>, int> GetActiveChain() const
    {
        return BuildChain(m_current);
    }

    TState *GetCurrentState() const { return m_current; }

    // True for a descendant too, not just an exact match.
    bool IsInState(TStateId id) const
    {
        auto entry = m_states.find(id);
        return entry != m_states.end() && IsAncestorOrSelf(entry->second.get(), m_current);
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
