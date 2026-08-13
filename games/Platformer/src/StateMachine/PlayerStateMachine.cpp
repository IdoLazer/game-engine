#include "PlayerStateMachine.h"
#include <algorithm>

// --- Setup ---

void PlayerStateMachine::RegisterState(PlayerStateId id, std::unique_ptr<PlayerState> state,
                                       std::optional<PlayerStateId> parent)
{
    if (parent)
        state->SetParent(m_states.at(*parent).get());

    m_states[id] = std::move(state);
}

// --- Control ---

void PlayerStateMachine::TransitionTo(PlayerStateId id)
{
    if (m_current && m_currentId == id) return;

    PlayerState *target = m_states.at(id).get();
    PlayerState *shared = FindCommonAncestor(m_current, target);

    // Everything from the shared ancestor upwards stays entered.
    for (PlayerState *state = m_current; state != shared; state = state->GetParent())
        state->Exit();

    // Set current before Enter() runs so a reentrant TransitionTo sees
    // consistent state and its result sticks once this call unwinds.
    m_currentId = id;
    m_current = target;

    int count = 0;
    std::array<PlayerState *, kMaxDepth> chain = BuildChain(target, count);

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

void PlayerStateMachine::Update(float deltaTime)
{
    PlayerState *active = m_current;
    int count = 0;
    std::array<PlayerState *, kMaxDepth> chain = BuildChain(active, count);

    for (int depth = 0; depth < count; ++depth)
    {
        if (m_current != active) return; // transitioned mid-update; the new state starts next frame
        chain[depth]->Update(deltaTime);
    }
}

// --- Rendering ---

void PlayerStateMachine::AdjustVisual(Engine::Vec2 &center, Engine::Vec2 &size) const
{
    int count = 0;
    std::array<PlayerState *, kMaxDepth> chain = BuildChain(m_current, count);

    for (int depth = 0; depth < count; ++depth)
        chain[depth]->AdjustVisual(center, size);
}

// --- Accessors ---

PlayerStateId PlayerStateMachine::GetCurrentId() const
{
    return m_currentId;
}

const char *PlayerStateMachine::GetCurrentName() const
{
    return m_current ? m_current->GetName() : "None";
}

// --- Internal ---

PlayerState *PlayerStateMachine::FindCommonAncestor(PlayerState *a, PlayerState *b) const
{
    for (PlayerState *state = a; state; state = state->GetParent())
    {
        if (IsAncestorOrSelf(state, b)) return state;
    }
    return nullptr;
}

bool PlayerStateMachine::IsAncestorOrSelf(const PlayerState *ancestor, PlayerState *state)
{
    for (PlayerState *current = state; current; current = current->GetParent())
    {
        if (current == ancestor) return true;
    }
    return false;
}

std::array<PlayerState *, PlayerStateMachine::kMaxDepth> PlayerStateMachine::BuildChain(PlayerState *state, int &outCount)
{
    std::array<PlayerState *, kMaxDepth> chain{};
    outCount = 0;

    for (PlayerState *current = state; current && outCount < kMaxDepth; current = current->GetParent())
        chain[outCount++] = current;

    std::reverse(chain.begin(), chain.begin() + outCount);
    return chain;
}
