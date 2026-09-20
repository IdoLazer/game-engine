#include "FalconStates.h"

using namespace Engine;

// --- Shared ---

void FalconState::Render() const
{
    m_falcon.DrawBody();
}

// --- On Shoulder ---

void Falcon::OnShoulderState::Enter()
{
    // An aim asked for while the falcon is away resumes the moment the falcon is back.
    m_falcon.ConsumeQueuedAimRequest();
    m_falcon.m_onReturnedEvent.Notify();
}

void Falcon::OnShoulderState::Update(float deltaTime)
{
    m_falcon.FollowPlayer(m_falcon.m_offsetFromPlayer);
}

bool Falcon::OnShoulderState::OnAimRequested()
{
    m_machine.TransitionTo(FalconStateId::Aiming);
    return true;
}

bool Falcon::OnShoulderState::OnGlideRequested()
{
    m_machine.TransitionTo(FalconStateId::Gliding);
    return true;
}

// --- Aiming ---

void Falcon::AimingState::Update(float deltaTime)
{
    m_falcon.AimAtCursor();
}

bool Falcon::AimingState::OnAimReleased()
{
    if (m_falcon.m_latchPoint)
        m_machine.TransitionTo(FalconStateId::Flying);
    else
        m_machine.TransitionTo(FalconStateId::OnShoulder);

    return true;
}

bool Falcon::AimingState::OnGlideRequested()
{
    // Remember the aim and pick it back up once the glide ends
    m_falcon.QueueAimRequest();
    return false;
}

void Falcon::AimingState::Render() const
{
    m_falcon.DrawBody();
    m_falcon.DrawAimOverlay();
}

// --- Flying ---

void Falcon::FlyingState::Update(float deltaTime)
{
    if (m_falcon.m_latchPoint && m_falcon.FlyTowards(*m_falcon.m_latchPoint, m_falcon.m_speed, deltaTime))
        m_machine.TransitionTo(FalconStateId::Latched);
}

bool Falcon::FlyingState::OnRetrieveRequested()
{
    m_falcon.m_latchPoint.reset();
    m_machine.TransitionTo(FalconStateId::Returning);
    return true;
}

// --- Returning ---

void Falcon::ReturningState::Update(float deltaTime)
{
    if (m_falcon.m_player && m_falcon.FlyTowards(m_falcon.ShoulderPosition(), m_falcon.m_retrieveSpeed, deltaTime))
        m_machine.TransitionTo(FalconStateId::OnShoulder);
}

// --- Latched ---

void Falcon::LatchedState::Enter()
{
    // Pointy end into the surface it hit.
    m_falcon.m_direction = m_falcon.m_latchDirection;
}

bool Falcon::LatchedState::OnRetrieveRequested()
{
    m_falcon.m_latchPoint.reset();
    m_machine.TransitionTo(FalconStateId::Returning);
    return true;
}

// --- Gliding ---

void Falcon::GlidingState::Update(float deltaTime)
{
    m_falcon.FollowPlayer(m_falcon.m_glideOffsetFromPlayer);
}

bool Falcon::GlidingState::OnGlideReleased()
{
    m_machine.TransitionTo(FalconStateId::OnShoulder);
    return true;
}

void Falcon::GlidingState::Render() const
{
    m_falcon.DrawGlidingBody();
}
