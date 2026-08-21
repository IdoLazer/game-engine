#include "Player.h"
#include "PlatformerWorld.h"
#include "Falcon.h"
#include "Commands/PlayerCommands.h"
#include "StateMachine/PlayerStates.h"
#include <algorithm>
#include <cmath>
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

// --- Lifecycle ---

void Player::Initialize()
{
    GridEntity::Initialize();
    m_halfExtents = GetGridSize() / 2.0f;

    m_jumpBufferTimer = Timer(m_jumpBufferTime, [this]() { m_jumpCommandQueue.Clear(); }, false);
    m_jumpBufferTimer.Stop();

    RegisterState<GroundedState>();
    RegisterState<OnWallState>();
    RegisterState<AirborneState>();
    RegisterState<JumpingState>(PlayerStateId::Airborne);
    RegisterState<WallJumpingState>(PlayerStateId::Jumping);
    RegisterState<FallingState>(PlayerStateId::Airborne);
    RegisterState<GlidingState>(PlayerStateId::Airborne);

    m_stateMachine.TransitionTo(PlayerStateId::Grounded);

    m_falconReturnedSubscription = m_falcon->OnReturned().Subscribe(this, &Player::OnFalconReturned);
}

void Player::Update(float deltaTime)
{
    m_jumpBufferTimer.Update(deltaTime);
    m_stateMachine.Update(deltaTime);
    HandleCollisions(deltaTime);
}

void Player::Render() const
{
    Vec2 center = m_worldPosition;
    Vec2 size = m_worldSize;

    m_stateMachine.AdjustVisual(center, size);

    // Stretch on the way up, squash on the way down, preserving area.
    float scale = m_velocity.y < 0.0f
        ? std::clamp(1.0f + (-m_velocity.y / m_jumpForce) * 0.25f, 1.0f, 1.25f)
        : std::clamp(1.0f - (m_velocity.y / m_jumpForce) * 0.15f, 0.85f, 1.0f);
    size.x /= scale;
    size.y *= scale;

    Renderer2D::DrawTile(center, size, m_color);
}

void Player::Destroy()
{
    GridEntity::Destroy();
    m_falconReturnedSubscription.Unsubscribe();
}

// --- Accessors ---

void Player::SetDirection(const Vec2 &dir)
{
    m_direction = dir;
    m_stateMachine.DirectionChanged(dir);
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

bool Player::CanGlide() const
{
    return m_falcon && m_falcon->IsOnShoulder();
}

// --- Input ---
// Every one of these is the current state's call to make - it knows what it can
// act on, and its parent picks up whatever it can't.

void Player::Jump()
{
    m_stateMachine.JumpPressed();
}

void Player::StopJump()
{
    m_stateMachine.JumpReleased();
    ClearBufferedJump(); // letting go cancels a jump that was waiting to fire
}

void Player::Glide()
{
    if (!CanGlide() && !m_glideCommandQueue.HasCommands())
    {
        m_glideCommandQueue.EnqueueCommand(std::make_unique<GlideCommand>(*this));
    }

    m_stateMachine.GlidePressed();
}

void Player::StopGlide()
{
    m_glideCommandQueue.Clear();
    m_stateMachine.GlideReleased();
}

// --- Jump Buffer ---

void Player::BufferJump()
{
    if (m_jumpCommandQueue.HasCommands()) return;

    m_jumpCommandQueue.EnqueueCommand(std::make_unique<JumpCommand>(*this));
    m_jumpBufferTimer.Reset();
}

void Player::ConsumeBufferedJump()
{
    if (!m_jumpCommandQueue.HasCommands()) return;

    m_jumpBufferTimer.Stop();
    m_jumpCommandQueue.DequeueCommand()->Execute();
}

void Player::ClearBufferedJump()
{
    m_jumpCommandQueue.Clear();
    m_jumpBufferTimer.Stop();
}

// --- Physics ---

void Player::ApplyGravity(float deltaTime, float maxSpeed, float scale)
{
    m_velocity.y += m_gravity * scale * deltaTime;
    if (m_velocity.y > maxSpeed)
        m_velocity.y = maxSpeed;
}

void Player::ApplyHorizontalAcceleration(float deltaTime, float accCoeff, float decCoeff)
{
    if (m_direction.x != 0)
    {
        m_velocity.x = std::clamp(m_velocity.x + m_direction.x * accCoeff * deltaTime, -m_speed, m_speed);
    }
    else if (m_velocity.x > 0)
    {
        m_velocity.x = std::max(0.0f, m_velocity.x - decCoeff * deltaTime);
    }
    else if (m_velocity.x < 0)
    {
        m_velocity.x = std::min(0.0f, m_velocity.x + decCoeff * deltaTime);
    }
}

void Player::LaunchJump()
{
    m_velocity.y = -m_jumpForce;
}

void Player::LaunchWallJump(int wallDirection)
{
    if (wallDirection == m_lastWallJumpDirection && GetGridPosition().y <= m_lastWallJumpHeight)
    {
        // No height gained from jumping off the same wall twice in a row.
        // TODO: This is a precaution. Proper way to deal with it is adjusting the different player
        // parameters to make it impossible to reach the same wall with the same height before
        // either landing or touching the other wall.
        // Ideally, after playtesting and tuning this could be safely removed.
        SetGridPosition(Vec2(GetGridPosition().x, m_lastWallJumpHeight));
    }

    float angleRad = m_wallJumpAngle * (std::numbers::pi_v<float> / 180.0f);
    m_velocity.x = -wallDirection * m_wallJumpForce * std::cos(angleRad);
    m_velocity.y = -m_wallJumpForce * std::sin(angleRad);

    m_lastWallJumpDirection = wallDirection;
    m_lastWallJumpHeight = GetGridPosition().y;
}

// Stops the player grinding into a wall they can't grab.
void Player::StopAgainstWall(int wallDirection)
{
    if (m_velocity.x != 0.0f && m_velocity.x * wallDirection >= 0.0f)
        m_velocity.x = 0.0f;
}

void Player::ForgetWallJumpHistory()
{
    m_lastWallJumpDirection = 0;
    m_lastWallJumpHeight = 0.0f;
}

// --- Collision ---

void Player::HandleCollisions(float deltaTime)
{
    Vec2 position = GetGridPosition();
    MoveAndSlide(position, deltaTime);
    SetGridPosition(position);

    m_stateMachine.ContactsResolved(ProbeContacts(position));
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

// Contact is an explicit query rather than something inferred from which direction the player
// happened to be moving, so it correctly persists as long as any part of the body still overlaps
// a solid, not just the center point.
PlayerContacts Player::ProbeContacts(const Vec2 &position) const
{
    Rect box(position, m_halfExtents);
    PlayerContacts contacts;

    // Ground is never reported while ascending. Without this, a jump that resolves within an
    // unusually short frame - before the player has moved meaningfully away from the ground -
    // would still find contact and cancel the jump that was just started.
    contacts.ground = m_velocity.y >= 0.0f && m_world->TouchesSolid(box, Vec2(0.0f, 1.0f));

    if (m_world->TouchesSolid(box, Vec2(-1.0f, 0.0f)))
        contacts.wall = -1;
    else if (m_world->TouchesSolid(box, Vec2(1.0f, 0.0f)))
        contacts.wall = 1;

    return contacts;
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


// --- Falcon Interaction ---

void Player::OnFalconReturned()
{
    if (m_glideCommandQueue.HasCommands())
    {
        m_glideCommandQueue.DequeueCommand()->Execute();
    }
}