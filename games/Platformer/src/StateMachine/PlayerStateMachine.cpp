#include "PlayerStateMachine.h"

void PlayerStateMachine::RegisterState(PlayerStateId id, std::unique_ptr<PlayerState> state)
{
    m_states[id] = std::move(state);
}

void PlayerStateMachine::TransitionTo(PlayerStateId id)
{
    if (m_current && m_currentId == id) return;

    if (m_current)
    {
        m_current->Exit();
    }

    // Set current before Enter() runs so a reentrant TransitionTo (e.g. from
    // a buffered command Enter() fires) sees consistent state and its result
    // sticks once this call unwinds.
    m_currentId = id;
    m_current = m_states.at(id).get();
    m_current->Enter();
}

void PlayerStateMachine::Update(float deltaTime)
{
    m_current->Update(deltaTime);
}

PlayerStateId PlayerStateMachine::GetCurrentId() const
{
    return m_currentId;
}

bool PlayerStateMachine::Is(PlayerStateId id) const
{
    return m_currentId == id;
}
