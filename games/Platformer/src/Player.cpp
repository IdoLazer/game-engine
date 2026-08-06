#include "Player.h"
#include "PlatformerWorld.h"
#include "Falcon.h"
#include <cmath>
#include <iostream>
#include <numbers>

// --- Type Registration ---
BEGIN_TYPE_REGISTER(Player)
    REGISTER_PROPERTY(float, Speed, &Player::m_speed)
    REGISTER_PROPERTY(float, AccelerationCoefficient, &Player::m_accCoeff)
    REGISTER_PROPERTY(float, DecelerationCoefficient, &Player::m_decCoeff)
    REGISTER_PROPERTY(float, AirAccelerationCoefficient, &Player::m_airAccCoeff)
    REGISTER_PROPERTY(float, AirDecelerationCoefficient, &Player::m_airDecCoeff)
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
        m_inWallJumpLock = false;
        if (m_startGlideCommandQueue.HasCommands())
        {
            m_startGlideCommandQueue.DequeueCommand()->Execute();
        }
    }, false);
}

void Player::Update(float deltaTime)
{
    m_coyoteTimer.Update(deltaTime);
    m_wallCoyoteTimer.Update(deltaTime);
    m_jumpBufferTimer.Update(deltaTime);
    m_minJumpTimer.Update(deltaTime);
    m_wallJumpLockTimer.Update(deltaTime);
    ApplyGravity(deltaTime);
    ApplyHorizontalMovement(deltaTime);
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
    if (m_isOnWall && (dir.x * m_wallDirection < 0))
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
    if (m_isGliding) return; // Can't jump while gliding

    // Case 1: Ground jump (includes coyote time window)
    if (m_isGrounded || m_inCoyoteTime)
    {
        m_velocity.y = -m_jumpForce;
        m_isJumping = true;
        m_inMinJump = true;
        m_minJumpTimer.Reset();
        m_isGrounded = false;
        m_inCoyoteTime = false;
        m_coyoteTimer.Stop();
    }
    // Case 2: Wall jump — launch on ballistic arc away from wall (includes coyote window)
    else if (m_isOnWall || m_inWallCoyoteTime)
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
    if (m_inWallJumpLock)
    {
        // If we're in wall jump lock, we can't glide yet — queue the glide command for execution after the lock expires
        if (!m_startGlideCommandQueue.HasCommands())
        {
            m_startGlideCommandQueue.EnqueueCommand(std::make_unique<GlideCommand>(*this));
        }
        return;
    }
    if (!m_isGrounded && m_falcon && m_falcon->IsOnShoulder())
    {
        m_isGliding = true;
        m_wallJumpCoasting = false; // Cancel wall jump coasting if we start gliding
        m_falcon->StartGlide();
    }
}

void Player::StopGlide()
{
    if (m_startGlideCommandQueue.HasCommands())
    {
        m_startGlideCommandQueue.Clear();
    }
    if (m_isGliding)
    {
        m_isGliding = false;
        m_falcon->StopGlide();
    }
}

// --- Physics ---

void Player::ApplyGravity(float deltaTime)
{
    m_isWallSliding = m_isOnWall && (m_velocity.x * m_wallDirection > 0);

    if (m_isWallSliding && m_velocity.y >= 0)
    {
        m_velocity.y += m_gravity * m_wallGravityScale * deltaTime;
        if (m_velocity.y > m_wallSlideMaxSpeed)
            m_velocity.y = m_wallSlideMaxSpeed;
    }
    else if (m_isGliding && m_velocity.y >= 0)
    {
        m_velocity.y += m_gravity * m_glideGravityScale * deltaTime;
        if (m_velocity.y > m_glideMaxSpeed)
            m_velocity.y = m_glideMaxSpeed;
    }
    else
    {
        m_velocity.y += m_gravity * deltaTime;
        if (m_velocity.y > m_maxFallSpeed)
            m_velocity.y = m_maxFallSpeed;
    }
}

// Horizontal movement flow:
//   Wall jump lock active → no input at all (ballistic arc)
//   Wall jump coasting    → no deceleration, but input can override
//   Normal                → accelerate toward input, decelerate when idle
void Player::ApplyHorizontalMovement(float deltaTime)
{
    // Phase 1: Lock — pure ballistic, player has no control
    if (m_inWallJumpLock) {
        return;
    }

    if (m_direction.x != 0)
    {
        // Phase 3 (or end of Phase 2): Player provides input — resume normal control
        m_wallJumpCoasting = false;
        float accCoeff = m_isGrounded ? m_accCoeff : m_airAccCoeff;
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
        // Phase 3: Normal deceleration (only when not coasting)
        float decCoeff = m_isGrounded ? m_decCoeff : m_isOnWall ? m_wallHitDecCoeff : m_airDecCoeff;
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
    // Phase 2: Coasting — no input and coasting flag set, velocity preserved as-is
}

// --- Collision ---

void Player::HandleCollisions(float deltaTime)
{
    Vec2 position = GetGridPosition();
    MoveAndSlide(position, deltaTime);
    SetGridPosition(position);

    // Must run before UpdateWallContact - it reads m_isGrounded.
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
    bool grounded = m_world->TouchesSolid(box, Vec2(0.0f, 1.0f));
    ChangeGroundedState(grounded);
}

void Player::UpdateWallContact(const Vec2 &position)
{
    // Don't detect wall contact during wall jump lock — we haven't cleared
    // the wall yet and the probe would prematurely end the lock.
    if (m_inWallJumpLock) return;

    // If grounded or gliding, wall contact should be false
    if (m_isGrounded || m_isGliding)
    {
        ChangeWallState(false, 0);
        return;
    }

    Rect box(position, m_halfExtents);
    bool leftSolid = m_world->TouchesSolid(box, Vec2(-1.0f, 0.0f));
    bool rightSolid = m_world->TouchesSolid(box, Vec2(1.0f, 0.0f));

    // Touching a wall while airborne is enough to count as "on wall" regardless of input
    // direction - this is what lets you wall-jump off a wall you're merely touching, not just
    // one you're actively sliding down. ChangeWallState is what guards against a ledge corner
    // hijacking your momentum on contact (see its comment).
    if (leftSolid && !m_isGrounded)
        ChangeWallState(true, -1);
    else if (rightSolid && !m_isGrounded)
        ChangeWallState(true, 1);
    else
        ChangeWallState(false, 0);
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

void Player::ChangeGroundedState(bool grounded)
{
    if (m_isGrounded == grounded) return;
    m_isGrounded = grounded;

    if (grounded)
    {
        ClearJumpState();
        ClearWallJumpTracking();
        m_isGliding = false;
        m_falcon->StopGlide();

        // Execute buffered jump immediately on landing
        if (m_jumpCommandQueue.HasCommands())
        {
            auto cmd = m_jumpCommandQueue.DequeueCommand();
            if (cmd) cmd->Execute();
            m_jumpBufferTimer.Stop();
        }
    }
    else
    {
        // Just left a ledge — start coyote time window
        m_inCoyoteTime = true;
        m_coyoteTimer.Reset();
    }
}

void Player::ChangeWallState(bool onWall, int direction)
{
    if (m_isOnWall == onWall && m_wallDirection == direction) return;

    if (onWall)
    {
        m_isOnWall = onWall;
        m_wallDirection = direction;

        // Push velocity into the wall on contact for a brief automatic wall-slide grace period -
        // but only if velocity is already carrying the player toward the wall (or is already
        // zero). Touching a wall while moving away from it - e.g. brushing a ledge corner while
        // walking off it - must not reverse that momentum; it still counts as "on wall" for
        // wall-jump purposes (above), just without hijacking existing motion.
        if (m_velocity.x != 0.0f && m_velocity.x * direction >= 0.0f)
            m_velocity.x = m_wallDirection * m_speed;

        ClearWallJumpTracking();
        m_falcon->StopGlide();

        // Execute buffered jump immediately on wall grab
        if (m_jumpCommandQueue.HasCommands())
        {
            auto cmd = m_jumpCommandQueue.DequeueCommand();
            if (cmd) cmd->Execute();
            m_jumpBufferTimer.Stop();
        }
    }
    else
    {
        // Save direction before resetting — needed by EnterWallJump during coyote time
        m_lastWallDirection = m_wallDirection;
        m_isOnWall = onWall;
        m_wallDirection = direction;

        m_inWallCoyoteTime = true;
        m_wallCoyoteTimer.Reset();
    }
}

void Player::EnterWallJump()
{
    // During coyote time m_wallDirection is 0, so use the remembered direction
    int jumpDirection = m_isOnWall ? m_wallDirection : m_lastWallDirection;

    float angleRad = m_wallJumpAngle * (std::numbers::pi_v<float> / 180.0f);
    m_velocity.x = -jumpDirection * m_wallJumpForce * std::cos(angleRad);
    m_velocity.y = -m_wallJumpForce * std::sin(angleRad);

    m_isJumping = true;
    m_inMinJump = true;
    m_minJumpTimer.Reset();

    // Lock phase: no player input for the duration of the timer
    m_inWallJumpLock = true;
    m_wallJumpCoasting = true;
    m_wallJumpLockTimer.Reset();

    ChangeWallState(false, 0);

    // ChangeWallState above would start wall coyote time — cancel it, we already jumped
    m_inWallCoyoteTime = false;
    m_wallCoyoteTimer.Stop();
}

void Player::ClearWallJumpTracking()
{
    m_inWallJumpLock = false;
    m_wallJumpCoasting = false;
    m_wallJumpLockTimer.Stop();
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
