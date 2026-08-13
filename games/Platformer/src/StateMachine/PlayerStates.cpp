#include "PlayerStates.h"
#include "../Commands/PlayerCommands.h"
#include "../Falcon.h"

using namespace Engine;

// --- Grounded ---

void Player::GroundedState::Enter()
{
    m_player.ForgetWallJumpHistory();
    m_player.ConsumeBufferedJump();
}

void Player::GroundedState::Update(float deltaTime)
{
    m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);
    m_player.ApplyHorizontalAcceleration(deltaTime, m_player.m_accCoeff, m_player.m_decCoeff);
}

bool Player::GroundedState::OnJumpPressed()
{
    m_player.LaunchJump();
    m_machine.TransitionTo(PlayerStateId::Jumping);
    return true;
}

bool Player::GroundedState::OnContacts(const PlayerContacts &contacts)
{
    if (!contacts.ground)
    {
        // Walked off a ledge. A deliberate jump leaves through OnJumpPressed
        // instead, so this is the only path that grants coyote time.
        m_machine.Get<FallingState>().GrantGroundCoyote();
        m_machine.TransitionTo(PlayerStateId::Falling);
    }
    else if (contacts.wall != 0)
    {
        m_player.StopAgainstWall(contacts.wall);
    }
    return true;
}

// --- On Wall ---

void Player::OnWallState::Enter()
{
    // Push velocity into the wall for a brief automatic wall-slide grace period,
    // but only if it already carries the player toward the wall (or is zero).
    // Brushing a wall while moving away from it - a ledge corner on the way off
    // a platform - must not reverse that momentum.
    if (m_player.m_velocity.x != 0.0f && m_player.m_velocity.x * m_wallDirection >= 0.0f)
        m_player.m_velocity.x = m_wallDirection * m_player.m_speed;

    m_player.ConsumeBufferedJump();
}

void Player::OnWallState::Exit()
{
    m_isWallSliding = false;
    m_wallDirection = 0;
}

void Player::OnWallState::Update(float deltaTime)
{
    m_isWallSliding = (m_player.m_velocity.x * m_wallDirection > 0);

    if (m_isWallSliding && m_player.m_velocity.y >= 0)
        m_player.ApplyGravity(deltaTime, m_player.m_wallSlideMaxSpeed, m_player.m_wallGravityScale);
    else
        m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);

    // Reuses air acceleration - there's no dedicated wall-accel coefficient,
    // only a dedicated deceleration one.
    m_player.ApplyHorizontalAcceleration(deltaTime, m_player.m_airAccCoeff, m_player.m_wallHitDecCoeff);
}

bool Player::OnWallState::OnJumpPressed()
{
    m_player.LaunchWallJump(m_wallDirection);
    m_machine.Get<AirborneState>().BeginWallJumpLock();
    m_machine.TransitionTo(PlayerStateId::Jumping);
    return true;
}

bool Player::OnWallState::OnGlidePressed()
{
    if (m_player.CanGlide())
        m_machine.TransitionTo(PlayerStateId::Gliding);

    return true;
}

bool Player::OnWallState::OnDirectionChanged(const Vec2 &direction)
{
    // Pushing away from the wall detaches immediately instead of decelerating.
    if (direction.x * m_wallDirection < 0)
        m_player.m_velocity.x = 0;

    return true;
}

bool Player::OnWallState::OnContacts(const PlayerContacts &contacts)
{
    if (contacts.ground)
        m_machine.TransitionTo(PlayerStateId::Grounded);
    else if (contacts.wall == 0)
        LetGoOfWall();
    else
        m_wallDirection = contacts.wall;

    return true;
}

void Player::OnWallState::LetGoOfWall()
{
    m_machine.Get<FallingState>().GrantWallCoyote(m_wallDirection);
    m_machine.TransitionTo(PlayerStateId::Falling);
}

void Player::OnWallState::AdjustVisual(Vec2 &center, Vec2 &size) const
{
    if (!m_isWallSliding) return;

    center.x += m_wallDirection * 0.025f;
    size.x -= 0.05f;
    size.y *= 1.0f + (0.05f / size.x); // preserve area
}

// --- Airborne ---

Player::AirborneState::AirborneState(Player &player, PlayerStateMachine &machine)
    : PlayerState(player, machine),
      m_lockTimer(player.m_wallJumpLockTime, [this]() { EndWallJumpLock(); }, false)
{
    m_lockTimer.Stop();
}

void Player::AirborneState::Exit()
{
    m_lockTimer.Stop();
    m_coasting = false;
    m_glideCommandQueue.Clear();
}

void Player::AirborneState::Update(float deltaTime)
{
    m_lockTimer.Update(deltaTime);
}

void Player::AirborneState::BeginWallJumpLock()
{
    m_lockTimer.Reset();
    m_coasting = true;
}

void Player::AirborneState::EndWallJumpLock()
{
    m_lockTimer.Stop(); // before the deferred glide runs, so it isn't deferred all over again
    if (m_glideCommandQueue.HasCommands())
        m_glideCommandQueue.DequeueCommand()->Execute();
}

void Player::AirborneState::ApplyAirControl(float deltaTime, float accCoeff, float decCoeff)
{
    if (m_lockTimer.IsActive()) return; // ballistic: no horizontal control at all

    if (m_player.m_direction.x != 0)
        m_coasting = false;
    else if (m_coasting)
        return; // keep the wall jump's momentum until there's input to override it

    m_player.ApplyHorizontalAcceleration(deltaTime, accCoeff, decCoeff);
}

bool Player::AirborneState::OnJumpPressed()
{
    // Nothing to jump from - remember it in case we land in time to use it.
    m_player.BufferJump();
    return true;
}

bool Player::AirborneState::OnGlidePressed()
{
    if (m_lockTimer.IsActive())
    {
        if (!m_glideCommandQueue.HasCommands())
            m_glideCommandQueue.EnqueueCommand(std::make_unique<GlideCommand>(m_player));
        return true;
    }

    if (m_player.CanGlide())
        m_machine.TransitionTo(PlayerStateId::Gliding);

    return true;
}

bool Player::AirborneState::OnGlideReleased()
{
    m_glideCommandQueue.Clear();
    return true;
}

bool Player::AirborneState::OnContacts(const PlayerContacts &contacts)
{
    if (contacts.ground)
    {
        m_machine.TransitionTo(PlayerStateId::Grounded);
        return true;
    }

    if (contacts.wall == 0) return true;

    // While locked, the wall we just launched from is still adjacent, and
    // re-grabbing it would cancel the jump outright. Only the opposite wall
    // ends the lock early.
    if (m_lockTimer.IsActive() && contacts.wall == m_player.m_lastWallJumpDirection)
        return true;

    // Merely touching a wall is enough to grab it, regardless of input direction
    // - that's what lets you wall-jump off a wall you brushed rather than one
    // you're actively sliding down.
    m_machine.Get<OnWallState>().GrabWall(contacts.wall);
    m_machine.TransitionTo(PlayerStateId::OnWall);
    return true;
}

// --- Jumping ---

Player::JumpingState::JumpingState(Player &player, PlayerStateMachine &machine)
    : AirborneSubState(player, machine),
      m_minJumpTimer(player.m_minJumpTime, [this]() { EndMinimumJump(); }, false)
{
    m_minJumpTimer.Stop();
}

void Player::JumpingState::Enter()
{
    m_minJumpTimer.Reset();
}

void Player::JumpingState::Exit()
{
    m_minJumpTimer.Stop();
    m_stopCommandQueue.Clear();
}

void Player::JumpingState::Update(float deltaTime)
{
    m_minJumpTimer.Update(deltaTime);

    m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);
    Airborne().ApplyAirControl(deltaTime, m_player.m_airAccCoeff, m_player.m_airDecCoeff);

    if (m_player.m_velocity.y > 0)
        m_machine.TransitionTo(PlayerStateId::Falling); // past the apex, nothing left to cut
}

bool Player::JumpingState::OnJumpReleased()
{
    if (m_minJumpTimer.IsActive())
    {
        // Too early to cut - run the release once the minimum arc is done.
        if (!m_stopCommandQueue.HasCommands())
            m_stopCommandQueue.EnqueueCommand(std::make_unique<JumpStopCommand>(m_player));
        return true;
    }

    m_player.m_velocity.y = 0;
    m_machine.TransitionTo(PlayerStateId::Falling);
    return true;
}

void Player::JumpingState::EndMinimumJump()
{
    m_minJumpTimer.Stop(); // before the deferred release runs, so it isn't deferred all over again
    if (m_stopCommandQueue.HasCommands())
        m_stopCommandQueue.DequeueCommand()->Execute();
}

// --- Falling ---

Player::FallingState::FallingState(Player &player, PlayerStateMachine &machine)
    : AirborneSubState(player, machine),
      m_coyoteTimer(0.0f, [this]() { m_coyoteJump = CoyoteJump::None; }, false)
{
    m_coyoteTimer.Stop();
}

void Player::FallingState::Exit()
{
    m_coyoteJump = CoyoteJump::None;
    m_coyoteTimer.Stop();
}

void Player::FallingState::Update(float deltaTime)
{
    m_coyoteTimer.Update(deltaTime);

    m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);
    Airborne().ApplyAirControl(deltaTime, m_player.m_airAccCoeff, m_player.m_airDecCoeff);
}

bool Player::FallingState::OnJumpPressed()
{
    switch (m_coyoteJump)
    {
    case CoyoteJump::Ground:
        m_player.LaunchJump();
        m_machine.TransitionTo(PlayerStateId::Jumping);
        return true;

    case CoyoteJump::Wall:
        m_player.LaunchWallJump(m_coyoteWallDirection);
        Airborne().BeginWallJumpLock();
        m_machine.TransitionTo(PlayerStateId::Jumping);
        return true;

    case CoyoteJump::None:
        return false; // nothing left to jump from - let Airborne buffer it
    }

    return false;
}

void Player::FallingState::GrantGroundCoyote()
{
    m_coyoteJump = CoyoteJump::Ground;
    m_coyoteTimer.SetInterval(m_player.m_coyoteTime);
    m_coyoteTimer.Reset();
}

void Player::FallingState::GrantWallCoyote(int wallDirection)
{
    m_coyoteJump = CoyoteJump::Wall;
    m_coyoteWallDirection = wallDirection;
    m_coyoteTimer.SetInterval(m_player.m_wallCoyoteTime);
    m_coyoteTimer.Reset();
}

// --- Gliding ---

void Player::GlidingState::Enter()
{
    Airborne().StopCoasting();
    m_player.m_falcon->StartGlide();
}

void Player::GlidingState::Exit()
{
    m_player.m_falcon->StopGlide();
}

void Player::GlidingState::Update(float deltaTime)
{
    // Glide gravity only applies once already falling. Gliding triggered while
    // still rising - right at a jump's apex, say - keeps normal gravity until
    // the player starts coming down.
    if (m_player.m_velocity.y >= 0)
        m_player.ApplyGravity(deltaTime, m_player.m_glideMaxSpeed, m_player.m_glideGravityScale);
    else
        m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);

    Airborne().ApplyAirControl(deltaTime, m_player.m_glideAccCoeff, m_player.m_glideDecCoeff);
}

bool Player::GlidingState::OnJumpPressed()
{
    return true; // hands are busy holding the falcon
}

bool Player::GlidingState::OnGlideReleased()
{
    m_machine.TransitionTo(PlayerStateId::Falling);
    return true;
}

bool Player::GlidingState::OnContacts(const PlayerContacts &contacts)
{
    if (contacts.ground || contacts.wall == 0)
        return false; // landing is handled the same way as for any airborne state

    // Gliding doesn't grab walls, it stops against them.
    m_player.StopAgainstWall(contacts.wall);
    if (contacts.wall != m_player.m_lastWallJumpDirection)
        m_player.ForgetWallJumpHistory();

    return true;
}
