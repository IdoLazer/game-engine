#include "Player.h"
#include "PlatformerWorld.h"
#include "Falcon.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <numbers>

// --- Type Registration ---
BEGIN_TYPE_REGISTER(Player)
    REGISTER_PROPERTY(float, Speed, &Player::m_speed)
    REGISTER_PROPERTY(float, AccelerationCoefficient, &Player::m_accCoeff)
    REGISTER_PROPERTY(float, DecelerationCoefficient, &Player::m_decCoeff)
    REGISTER_PROPERTY(float, AirAccelerationCoefficient, &Player::m_airAccCoeff)
    REGISTER_PROPERTY(float, AirDecelerationCoefficient, &Player::m_airDecCoeff)
    REGISTER_PROPERTY(float, GlideAccelerationCoefficient, &Player::m_glideAccCoeff)
    REGISTER_PROPERTY(float, GlideDecelerationCoefficient, &Player::m_glideDecCoeff)
    REGISTER_PROPERTY(float, WallHitDecelerationCoefficient, &Player::m_wallHitDecCoeff)
    REGISTER_PROPERTY(float, JumpForce, &Player::m_jumpForce)
    REGISTER_PROPERTY(float, Gravity, &Player::m_gravity)
    REGISTER_PROPERTY(float, CoyoteTime, &Player::m_coyoteTime)
    REGISTER_PROPERTY(float, WallCoyoteTime, &Player::m_wallCoyoteTime)
    REGISTER_PROPERTY(float, JumpBufferTime, &Player::m_jumpBufferTime)
    REGISTER_PROPERTY(float, MinJumpTime, &Player::m_minJumpTime)
    REGISTER_PROPERTY(float, WallJumpLockTime, &Player::m_wallJumpLockTime)
    REGISTER_PROPERTY(float, WallJumpForce, &Player::m_wallJumpForce)
    REGISTER_PROPERTY(float, WallJumpAngle, &Player::m_wallJumpAngle)
    REGISTER_PROPERTY(float, MaxFallSpeed, &Player::m_maxFallSpeed)
    REGISTER_PROPERTY(float, WallSlideMaxSpeed, &Player::m_wallSlideMaxSpeed)
    REGISTER_PROPERTY(float, WallGravityScale, &Player::m_wallGravityScale)
    REGISTER_PROPERTY(float, GlideMaxSpeed, &Player::m_glideMaxSpeed)
    REGISTER_PROPERTY(float, GlideGravityScale, &Player::m_glideGravityScale)
END_TYPE_REGISTER()

using namespace Engine;

// --- Types and Structs ---
class JumpCommand : public Engine::Command
{
public:
    JumpCommand(Player &player) : m_player(player) {}
    void Execute() override { m_player.Jump(); }
    const char* GetName() const override { return "JumpCommand"; }
private:
    Player &m_player;
};

class JumpStopCommand : public Engine::Command
{
public:
    JumpStopCommand(Player &player) : m_player(player) {}
    void Execute() override { m_player.StopJump(); }
    const char* GetName() const override { return "JumpStopCommand"; }
private:
    Player &m_player;
};

class GlideCommand : public Engine::Command
{
public:
    GlideCommand(Player &player) : m_player(player) {}
    void Execute() override { m_player.Glide(); }
    const char* GetName() const override { return "GlideCommand"; }
private:
    Player &m_player;
};

// --- Lifecycle ---
void Player::Initialize()
{
    GridEntity::Initialize();
    m_halfExtents = GetGridSize() / 2.0f;
    m_coyoteTimer = Timer(m_coyoteTime, [this]() {
        m_inCoyoteTime = false;
    }, false);
    m_wallCoyoteTimer = Timer(m_wallCoyoteTime, [this]() {
        m_inWallCoyoteTime = false;
    }, false);
    m_jumpBufferTimer = Timer(m_jumpBufferTime, [this]() {
        m_jumpCommandQueue.Clear();
    }, false);
    m_minJumpTimer = Timer(m_minJumpTime, [this]() {
        m_inMinJump = false;
        if (m_jumpStopCommandQueue.HasCommands())
        {
            m_jumpStopCommandQueue.DequeueCommand()->Execute();
        }
    }, false);
    m_wallJumpLockTimer = Timer(m_wallJumpLockTime, [this]() {
        // Guaranteed to still be WallJumpLock here - any earlier exit (ground
        // or opposite-wall contact) would have stopped this timer already.
        m_stateMachine.TransitionTo(PlayerStateId::Airborne);
        if (m_startGlideCommandQueue.HasCommands())
        {
            m_startGlideCommandQueue.DequeueCommand()->Execute();
        }
    }, false);

    m_stateMachine.RegisterState(PlayerStateId::Grounded, std::make_unique<GroundedState>(*this));
    m_stateMachine.RegisterState(PlayerStateId::Airborne, std::make_unique<AirborneState>(*this));
    m_stateMachine.RegisterState(PlayerStateId::OnWall, std::make_unique<OnWallState>(*this));
    m_stateMachine.RegisterState(PlayerStateId::Gliding, std::make_unique<GlidingState>(*this));
    m_stateMachine.RegisterState(PlayerStateId::WallJumpLock, std::make_unique<WallJumpLockState>(*this));
    m_stateMachine.TransitionTo(PlayerStateId::Grounded);
}

void Player::Update(float deltaTime)
{
    m_coyoteTimer.Update(deltaTime);
    m_wallCoyoteTimer.Update(deltaTime);
    m_jumpBufferTimer.Update(deltaTime);
    m_minJumpTimer.Update(deltaTime);
    m_wallJumpLockTimer.Update(deltaTime);

    m_stateMachine.Update(deltaTime);
    if (m_isJumping && m_velocity.y > 0) m_isJumping = false; // If we start falling, we're no longer in the jump state
    HandleCollisions(deltaTime);
}

void Player::Render() const
{
    Vec2 center = m_worldPosition;
    Vec2 size = m_worldSize;
    Color color = m_color;

    if (m_isWallSliding)
    {
        center.x += m_wallDirection * 0.025f;
        size.x -= 0.05f;
        size.y *= 1.0f + (0.05f / size.x); // preserve area
    }
    if (m_isJumping)
    {
        float stretchFactor = std::clamp(1.0f + (-m_velocity.y / m_jumpForce) * 0.25f, 1.0f, 1.25f);
        size.x /= stretchFactor; // preserve area
        size.y *= stretchFactor;
    }
    else if (m_velocity.y > 0.0f)
    {
        float shrinkFactor = std::clamp(1.0f - (m_velocity.y / m_jumpForce) * 0.15f, 0.85f, 1.0f);
        size.x /= shrinkFactor; // preserve area
        size.y *= shrinkFactor;
    }

    Renderer2D::DrawTile(center, size, color);
}

void Player::Destroy()
{
    GridEntity::Destroy();
}

// --- Accessors ---

void Player::SetDirection(const Vec2 &dir)
{
    m_direction = dir;

    // If we're on a wall, we want immediate detachment when pushing away
    if (m_stateMachine.Is(PlayerStateId::OnWall) && (dir.x * m_wallDirection < 0))
    {
        m_velocity.x = 0;
    }
}

void Player::SetWorld(PlatformerWorld *world)
{
    m_world = world;
}

void Player::SetFalcon(Falcon *falcon)
{
    m_falcon = falcon;
    m_falcon->SetPlayer(this);
}

bool Player::IsJumping() const
{
    return m_isJumping;
}

// --- Jump Actions ---
// Jump flow:
//   1. Ground/coyote jump: standard jump, clears grounded state
//   2. Wall jump: launches away from wall, enters lock → coast → normal control
//   3. Buffered jump: queued for execution on next landing or wall grab

void Player::Jump()
{
    if (m_stateMachine.Is(PlayerStateId::Gliding)) return; // Can't jump while gliding

    // Case 1: Ground jump (includes coyote time window)
    if (m_stateMachine.Is(PlayerStateId::Grounded) || m_inCoyoteTime)
    {
        m_velocity.y = -m_jumpForce;
        m_isJumping = true;
        m_inMinJump = true;
        m_minJumpTimer.Reset();
        // GroundedState::Exit() doesn't start coyote time (only
        // UpdateGroundedState's own leave-the-ground detection does) - so
        // transitioning here doesn't grant a free extra coyote window.
        m_stateMachine.TransitionTo(PlayerStateId::Airborne);
        m_inCoyoteTime = false;
        m_coyoteTimer.Stop();
    }
    // Case 2: Wall jump — launch on ballistic arc away from wall (includes coyote window)
    else if (m_stateMachine.Is(PlayerStateId::OnWall) || m_inWallCoyoteTime)
    {
        EnterWallJump();
    }
    // Case 3: Can't jump now — buffer input for later
    else
    {
        if (!m_jumpCommandQueue.HasCommands())
        {
            m_jumpCommandQueue.EnqueueCommand(std::make_unique<JumpCommand>(*this));
            m_jumpBufferTimer.Reset();
        }
        return;
    }

    // Clear any pending jump stop commands — we just initiated a jump
    if (m_jumpStopCommandQueue.HasCommands())
    {
        m_jumpStopCommandQueue.Clear();
    }
}

void Player::StopJump()
{
    if (m_isJumping)
    {
        if (m_inMinJump && !m_jumpStopCommandQueue.HasCommands())
        {
            // Still in minimum jump time — defer the stop until min jump expires
            m_jumpStopCommandQueue.EnqueueCommand(std::make_unique<JumpStopCommand>(*this));
        }
        else
        {
            m_velocity.y = 0;
            m_isJumping = false;
        }
    }
    if (m_jumpCommandQueue.HasCommands())
    {
        m_jumpCommandQueue.Clear();
        m_jumpBufferTimer.Stop();
    }
}

void Player::Glide()
{
    if (m_stateMachine.Is(PlayerStateId::WallJumpLock))
    {
        // If we're in wall jump lock, we can't glide yet — queue the glide command for execution after the lock expires
        if (!m_startGlideCommandQueue.HasCommands())
        {
            m_startGlideCommandQueue.EnqueueCommand(std::make_unique<GlideCommand>(*this));
        }
        return;
    }
    if (!m_stateMachine.Is(PlayerStateId::Grounded) && m_falcon && m_falcon->IsOnShoulder())
    {
        m_stateMachine.TransitionTo(PlayerStateId::Gliding);
    }
}

void Player::StopGlide()
{
    if (m_startGlideCommandQueue.HasCommands())
    {
        m_startGlideCommandQueue.Clear();
    }
    if (m_stateMachine.Is(PlayerStateId::Gliding))
    {
        m_stateMachine.TransitionTo(PlayerStateId::Airborne);
    }
}

// --- Physics ---

void Player::ApplyGravity(float deltaTime, float maxSpeed, float scale)
{
    m_velocity.y += m_gravity * scale * deltaTime;
    if (m_velocity.y > maxSpeed)
        m_velocity.y = maxSpeed;
}

// Horizontal movement flow:
//   Wall jump coasting → no deceleration, but input can override
//   Normal             → accelerate toward input, decelerate when idle
// (Wall jump lock's "no input at all" phase is its own state - WallJumpLockState
// simply never calls this.)
void Player::ApplyHorizontalAcceleration(float deltaTime, float accCoeff, float decCoeff)
{
    if (m_direction.x != 0)
    {
        m_wallJumpCoasting = false;
        m_velocity.x += m_direction.x * accCoeff * deltaTime;
        if (m_velocity.x > m_speed)
        {
            m_velocity.x = m_speed;
        }
        else if (m_velocity.x < -m_speed)
        {
            m_velocity.x = -m_speed;
        }
    }
    else if (!m_wallJumpCoasting)
    {
        if (m_velocity.x > 0)
        {
            m_velocity.x -= decCoeff * deltaTime;
            if (m_velocity.x < 0) m_velocity.x = 0;
        }
        else if (m_velocity.x < 0)
        {
            m_velocity.x += decCoeff * deltaTime;
            if (m_velocity.x > 0) m_velocity.x = 0;
        }
    }
}

// --- Collision ---

void Player::HandleCollisions(float deltaTime)
{
    Vec2 position = GetGridPosition();
    MoveAndSlide(position, deltaTime);
    SetGridPosition(position);

    // Must run before UpdateWallContact - it reads whether we're Grounded.
    UpdateGroundedState(position);
    UpdateWallContact(position);
    CheckChangeLevel(position);
}

// Sweeps the full combined velocity for this frame in one pass, clipping and sliding along
// whatever it hits rather than resolving X and Y as two independent 1D sweeps - a separated
// sweep can let a diagonal approach slip into a corner before either axis, checked against the
// other's stale position, ever sees it coming.
void Player::MoveAndSlide(Vec2 &position, float deltaTime)
{
    Vec2 remaining = m_velocity * deltaTime;

    constexpr int kMaxSlideIterations = 4;
    for (int i = 0; i < kMaxSlideIterations && remaining != Vec2::Zero; ++i)
    {
        Rect box(position, m_halfExtents);
        SweepHit hit = m_world->SweepSolid(box, remaining);
        position += remaining * hit.t;

        if (!hit.hit)
            break;

        // Vertical hits stop outright (floor/ceiling). Horizontal hits deliberately leave
        // velocity.x untouched - m_wallHitDecCoeff handles the slowdown next frame instead of
        // an instant stop.
        if (hit.normal.y != 0.0f)
            m_velocity.y = 0.0f;

        // Slide: keep only the leftover movement perpendicular to what was hit.
        Vec2 leftover = remaining * (1.0f - hit.t);
        remaining = leftover - hit.normal * leftover.Dot(hit.normal);
    }
}

// Grounded is its own explicit query - not inferred from which direction the player happened
// to be moving - so it correctly persists as long as any part of the body still overlaps solid
// ground below, not just the center point.
void Player::UpdateGroundedState(const Vec2 &position)
{
    // Never touched while ascending. Without this, a jump that resolves within an unusually
    // short frame - before the player has moved meaningfully away from the ground - could still
    // find contact via TouchesSolid and immediately clear the jump state that was just set.
    if (m_velocity.y < 0.0f)
        return;

    Rect box(position, m_halfExtents);
    if (m_world->TouchesSolid(box, Vec2(0.0f, 1.0f)))
    {
        // Landing always wins, regardless of what was current - including
        // cutting a wall-jump lock short (WallJumpLockState::Exit() stops
        // its timer) instead of the two coexisting.
        m_stateMachine.TransitionTo(PlayerStateId::Grounded);
    }
    else if (m_stateMachine.Is(PlayerStateId::Grounded))
    {
        // Just walked off a ledge - not an explicit jump, Jump() transitions
        // straight to Airborne itself and skips this - so start coyote time.
        m_inCoyoteTime = true;
        m_coyoteTimer.Reset();
        m_stateMachine.TransitionTo(PlayerStateId::Airborne);
    }
    // Otherwise (not grounded, and wasn't a moment ago) leave whatever state
    // is current alone - it's OnWall/Gliding/WallJumpLock/Airborne for its
    // own reasons, none of which this probe should override.
}

void Player::UpdateWallContact(const Vec2 &position)
{
    Rect box(position, m_halfExtents);
    bool leftSolid = m_world->TouchesSolid(box, Vec2(-1.0f, 0.0f));
    bool rightSolid = m_world->TouchesSolid(box, Vec2(1.0f, 0.0f));
    int direction = leftSolid ? -1 : (rightSolid ? 1 : 0);

    if (m_stateMachine.Is(PlayerStateId::WallJumpLock))
    {
        // Ignore the wall we just launched from - we're still physically
        // adjacent to it until we've moved away, and re-grabbing it would
        // cancel the jump outright. Only the opposite wall (a wall jump
        // straight into another wall) ends the lock early.
        if (direction != 0 && direction != m_lastWallJumpDirection)
        {
            m_wallDirection = direction;
            m_stateMachine.TransitionTo(PlayerStateId::OnWall);
        }
        return;
    }

    bool suppressed = m_stateMachine.Is(PlayerStateId::Grounded) || m_stateMachine.Is(PlayerStateId::Gliding);

    if (direction != 0 && suppressed)
    {
        // Grounded/gliding suppresses wall-grab, but still stops the player
        // sliding into the wall and resets stale wall-jump-climb tracking if
        // it's now a different wall than the one last jumped from.
        if (m_velocity.x != 0.0f && m_velocity.x * direction >= 0.0f)
            m_velocity.x = 0.0f;
        if (m_lastWallJumpDirection != direction)
        {
            m_lastWallJumpDirection = 0;
            m_lastWallJumpHeight = 0.0f;
        }
    }
    else if (direction != 0)
    {
        // Touching a wall while airborne is enough to count as "on wall"
        // regardless of input direction - this is what lets you wall-jump
        // off a wall you're merely touching, not just one you're actively
        // sliding down.
        m_wallDirection = direction;
        m_stateMachine.TransitionTo(PlayerStateId::OnWall);
    }
    else if (m_stateMachine.Is(PlayerStateId::OnWall))
    {
        m_stateMachine.TransitionTo(PlayerStateId::Airborne);
    }
}

void Player::CheckChangeLevel(const Engine::Vec2 &position)
{
    Vec2 cell = GetGrid()->GetCellFromGridPosition(position);
    int row = static_cast<int>(cell.y);

    if (m_world->IsNextLevel(cell))
    {
        m_velocity.x = 0;
        m_nextLevelEvent.Notify(row);
    }
    else if (m_world->IsPreviousLevel(cell))
    {
        m_velocity.x = 0;
        m_previousLevelEvent.Notify(row);
    }
    else if (m_world->IsDeadly(cell))
    {
        m_reloadLevelEvent.Notify();
    }
}

// --- State Transitions ---

// Wall jumps can launch from an actual wall contact or from the remembered
// direction during wall coyote time, in which case m_wallDirection is
// already 0 - hence the ternary.
void Player::EnterWallJump()
{
    int jumpDirection = m_stateMachine.Is(PlayerStateId::OnWall) ? m_wallDirection : m_lastWallDirection;

    if (jumpDirection == m_lastWallJumpDirection && GetGridPosition().y <= m_lastWallJumpHeight)
    {
        // Prevent consecutive wall jumps from the same wall to climb it
        SetGridPosition(Vec2(GetGridPosition().x, m_lastWallJumpHeight + 0.01f));
    }

    float angleRad = m_wallJumpAngle * (std::numbers::pi_v<float> / 180.0f);
    m_velocity.x = -jumpDirection * m_wallJumpForce * std::cos(angleRad);
    m_velocity.y = -m_wallJumpForce * std::sin(angleRad);

    m_isJumping = true;
    m_inMinJump = true;
    m_minJumpTimer.Reset();

    // Remember the direction and height of this jump to prevent consecutive wall jumps from the same wall
    m_lastWallJumpDirection = jumpDirection;
    m_lastWallJumpHeight = GetGridPosition().y;

    // If we were on a wall, this fires OnWallState::Exit() (which would
    // start wall coyote time) immediately followed by WallJumpLockState::
    // Enter() (which cancels it right back out) - same net effect as
    // deliberately jumping consuming any coyote grace, just driven by the
    // ordinary transition machinery instead of a special-cased call.
    m_stateMachine.TransitionTo(PlayerStateId::WallJumpLock);
}

void Player::ClearWallJumpTracking()
{
    m_wallJumpCoasting = false;
    m_wallJumpLockTimer.Stop();
}

void Player::ExecuteBufferedJump()
{
    if (m_jumpCommandQueue.HasCommands())
    {
        auto cmd = m_jumpCommandQueue.DequeueCommand();
        if (cmd) cmd->Execute();
        m_jumpBufferTimer.Stop();
    }
}

void Player::ClearJumpState()
{
    m_isJumping = false;
    m_inMinJump = false;
    m_minJumpTimer.Stop();
    m_inCoyoteTime = false;
    m_coyoteTimer.Stop();
    m_inWallCoyoteTime = false;
    m_wallCoyoteTimer.Stop();
}

// --- Movement States ---

void Player::GroundedState::Enter()
{
    m_player.ClearJumpState();
    m_player.ClearWallJumpTracking();
    m_player.m_lastWallJumpDirection = 0;
    m_player.m_lastWallJumpHeight = 0.0f;
    m_player.m_falcon->StopGlide();
    m_player.ExecuteBufferedJump();
}

void Player::GroundedState::Update(float deltaTime)
{
    // Wall-jump lock is its own state now, so there's nothing to guard
    // against here - GroundedState can only be current when it isn't.
    m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);
    m_player.ApplyHorizontalAcceleration(deltaTime, m_player.m_accCoeff, m_player.m_decCoeff);
}

void Player::AirborneState::Update(float deltaTime)
{
    m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);
    m_player.ApplyHorizontalAcceleration(deltaTime, m_player.m_airAccCoeff, m_player.m_airDecCoeff);
}

void Player::OnWallState::Enter()
{
    // Push velocity into the wall on contact for a brief automatic wall-slide
    // grace period - but only if velocity is already carrying the player
    // toward the wall (or is already zero). Touching a wall while moving
    // away from it - e.g. brushing a ledge corner while walking off it -
    // must not reverse that momentum.
    if (m_player.m_velocity.x != 0.0f && m_player.m_velocity.x * m_player.m_wallDirection >= 0.0f)
        m_player.m_velocity.x = m_player.m_wallDirection * m_player.m_speed;

    m_player.ClearWallJumpTracking();
    m_player.m_falcon->StopGlide();
    m_player.ExecuteBufferedJump();
}

void Player::OnWallState::Exit()
{
    m_player.m_isWallSliding = false;

    // Save direction before resetting — needed by EnterWallJump during coyote time
    m_player.m_lastWallDirection = m_player.m_wallDirection;
    m_player.m_wallDirection = 0;

    m_player.m_inWallCoyoteTime = true;
    m_player.m_wallCoyoteTimer.Reset();
}

void Player::OnWallState::Update(float deltaTime)
{
    m_player.m_isWallSliding = (m_player.m_velocity.x * m_player.m_wallDirection > 0);

    if (m_player.m_isWallSliding && m_player.m_velocity.y >= 0)
        m_player.ApplyGravity(deltaTime, m_player.m_wallSlideMaxSpeed, m_player.m_wallGravityScale);
    else
        m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);

    // Reuses air acceleration - there's no dedicated wall-accel coefficient,
    // only a dedicated deceleration one (m_wallHitDecCoeff).
    m_player.ApplyHorizontalAcceleration(deltaTime, m_player.m_airAccCoeff, m_player.m_wallHitDecCoeff);
}

void Player::GlidingState::Enter()
{
    m_player.m_wallJumpCoasting = false;
    m_player.m_falcon->StartGlide();
}

void Player::GlidingState::Exit()
{
    m_player.m_falcon->StopGlide();
}

void Player::GlidingState::Update(float deltaTime)
{
    // Glide gravity only applies once already falling; still-ascending (e.g.
    // gliding triggered right at a jump's apex) keeps normal gravity until
    // velocity.y turns non-negative.
    if (m_player.m_velocity.y >= 0)
        m_player.ApplyGravity(deltaTime, m_player.m_glideMaxSpeed, m_player.m_glideGravityScale);
    else
        m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);

    m_player.ApplyHorizontalAcceleration(deltaTime, m_player.m_glideAccCoeff, m_player.m_glideDecCoeff);
}

void Player::WallJumpLockState::Enter()
{
    m_player.m_wallJumpCoasting = true;
    m_player.m_wallJumpLockTimer.Reset();

    // Entering a wall jump always consumes any pending wall-coyote grace -
    // OnWallState::Exit() (if we were on a wall) may have just started one.
    m_player.m_inWallCoyoteTime = false;
    m_player.m_wallCoyoteTimer.Stop();
}

void Player::WallJumpLockState::Exit()
{
    // Covers both the natural timer expiry and being cut short by ground or
    // opposite-wall contact - safe to call even mid-callback (Timer::Stop()
    // is just a flag write).
    m_player.m_wallJumpLockTimer.Stop();
}

void Player::WallJumpLockState::Update(float deltaTime)
{
    // Ballistic - gravity applies normally, no horizontal control at all.
    m_player.ApplyGravity(deltaTime, m_player.m_maxFallSpeed);
}
