#include "Player.h"
#include "PlatformerWorld.h"
#include "PlatformerInputManager.h"
#include <cmath>
#include <iostream>
#include <numbers>

// --- Type Registration ---
BEGIN_TYPE_REGISTER(Player)
    REGISTER_PROPERTY(float, Speed, &Player::m_speed)
    REGISTER_PROPERTY(float, AccelerationCoefficient, &Player::m_accCoeff)
    REGISTER_PROPERTY(float, DecelerationCoefficient, &Player::m_decCoeff)
    REGISTER_PROPERTY(float, JumpForce, &Player::m_jumpForce)
    REGISTER_PROPERTY(float, Gravity, &Player::m_gravity)
    REGISTER_PROPERTY(float, CoyoteTime, &Player::m_coyoteTime)
    REGISTER_PROPERTY(float, JumpBufferTime, &Player::m_jumpBufferTime)
    REGISTER_PROPERTY(float, MinJumpTime, &Player::m_minJumpTime)
    REGISTER_PROPERTY(float, WallJumpLockTime, &Player::m_wallJumpLockTime)
    REGISTER_PROPERTY(float, WallJumpForce, &Player::m_wallJumpForce)
    REGISTER_PROPERTY(float, WallJumpAngle, &Player::m_wallJumpAngle)
    REGISTER_PROPERTY(float, MaxFallSpeed, &Player::m_maxFallSpeed)
    REGISTER_PROPERTY(float, WallSlideMaxSpeed, &Player::m_wallSlideMaxSpeed)
    REGISTER_PROPERTY(float, WallGravityScale, &Player::m_wallGravityScale)
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

// --- Lifecycle ---
void Player::Initialize()
{
    GridEntity::Initialize();
    m_playerBoundingBox[0] = -GetGridSize() / 2.0f;
    m_playerBoundingBox[1] = GetGridSize() / 2.0f;
    m_inputManager = std::make_unique<PlatformerInputManager>(*this);
    m_coyoteTimer = Timer(m_coyoteTime, [this]() {
        m_inCoyoteTime = false;
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
    }, false);
}

void Player::Update(float deltaTime)
{
    m_coyoteTimer.Update(deltaTime);
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
    Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);

    // Debug: Draw bounding box
    Vec2 center = GetGridPosition();
    Vec2 topLeft = center + m_playerBoundingBox[0];
    Vec2 bottomRight = center + m_playerBoundingBox[1];
    Vec2 topLeftWorld = GetGrid()->GridToWorld(topLeft);
    Vec2 bottomRightWorld = GetGrid()->GridToWorld(bottomRight);
    Vec2 size = bottomRightWorld - topLeftWorld;
    Renderer2D::DrawRectOutline(m_worldPosition, size, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

void Player::Destroy()
{
    GridEntity::Destroy();
}

// --- Accessors ---

void Player::SetDirection(const Vec2 &dir)
{
    m_direction = dir;
}

void Player::SetWorld(PlatformerWorld *world)
{
    m_world = world;
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
    // Case 2: Wall jump — launch on ballistic arc away from wall
    else if (m_isOnWall)
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

// --- Physics ---

void Player::ApplyGravity(float deltaTime)
{
    if (!m_isOnWall)
    {
        m_velocity.y += m_gravity * deltaTime;
        if (m_velocity.y > m_maxFallSpeed)
        {
            m_velocity.y = m_maxFallSpeed;
        }
    }
    else
    {
        if (m_velocity.y < 0) // If player is moving upward while on the wall, apply normal gravity to allow jumping up
        {
            m_velocity.y += m_gravity * deltaTime;
        }
        else // If player is moving downward or stationary while on the wall, apply reduced gravity for sliding effect
        {
            m_velocity.y += m_gravity * m_wallGravityScale * deltaTime;
            if (m_velocity.y > m_wallSlideMaxSpeed)
            {
                m_velocity.y = m_wallSlideMaxSpeed;
            }
        }
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

        m_velocity.x += m_direction.x * m_accCoeff * deltaTime;
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
        if (m_velocity.x > 0)
        {
            m_velocity.x -= m_decCoeff * deltaTime;
            if (m_velocity.x < 0) m_velocity.x = 0;
        }
        else if (m_velocity.x < 0)
        {
            m_velocity.x += m_decCoeff * deltaTime;
            if (m_velocity.x > 0) m_velocity.x = 0;
        }
    }
    // Phase 2: Coasting — no input and coasting flag set, velocity preserved as-is
}

// --- Collision ---
// Collision resolution order: horizontal → vertical → wall adjacency probe.
// Horizontal first so wall sliding works correctly with vertical checks.

void Player::HandleCollisions(float deltaTime)
{
    Vec2 currentPos = GetGridPosition();
    Vec2 newGridPos = currentPos + m_velocity * deltaTime;

    ResolveHorizontalCollisions(currentPos, newGridPos);
    ResolveVerticalCollisions(currentPos, newGridPos);

    SetGridPosition(newGridPos);
    UpdateWallContact(newGridPos);
    CheckChangeLevel(newGridPos);
}

void Player::ResolveHorizontalCollisions(const Vec2 &currentPos, Vec2 &newGridPos)
{
    // Left collision
    if (m_velocity.x < 0)
    {
        Vec2 probePoint = Vec2(newGridPos.x + m_playerBoundingBox[0].x, currentPos.y);
        Vec2 cell = GetGrid()->GetCellFromGridPosition(probePoint);
        if (m_world->IsSolid(cell))
        {
            m_velocity.x = 0;
            newGridPos.x = (cell.x + 0.5f) - m_playerBoundingBox[0].x;
        }
    }
    // Right collision
    if (m_velocity.x > 0)
    {
        Vec2 probePoint = Vec2(newGridPos.x + m_playerBoundingBox[1].x, currentPos.y);
        Vec2 cell = GetGrid()->GetCellFromGridPosition(probePoint);
        if (m_world->IsSolid(cell))
        {
            m_velocity.x = 0;
            newGridPos.x = (cell.x - 0.5f) - m_playerBoundingBox[1].x;
        }
    }
}

void Player::ResolveVerticalCollisions(const Vec2 &currentPos, Vec2 &newGridPos)
{
    // Upward collision
    if (m_velocity.y < 0)
    {
        Vec2 probePoint = Vec2(currentPos.x, newGridPos.y + m_playerBoundingBox[0].y);
        Vec2 cell = GetGrid()->GetCellFromGridPosition(probePoint);
        if (m_world->IsSolid(cell))
        {
            m_velocity.y = 0;
            newGridPos.y = (cell.y + 0.5f) - m_playerBoundingBox[0].y;
        }
    }
    // Downward collision — also manages grounded state
    else if (m_velocity.y >= 0)
    {
        Vec2 probePoint = Vec2(currentPos.x, newGridPos.y + m_playerBoundingBox[1].y);
        Vec2 cell = GetGrid()->GetCellFromGridPosition(probePoint);
        if (m_world->IsSolid(cell))
        {
            m_velocity.y = 0;
            newGridPos.y = (cell.y - 0.5f) - m_playerBoundingBox[1].y;
            ChangeGroundedState(true);
        }
        else
        {
            ChangeGroundedState(false);
        }
    }
}

void Player::UpdateWallContact(const Vec2 &position)
{
    // Don't detect wall contact during wall jump lock — we haven't cleared
    // the wall yet and the probe would prematurely end the lock.
    if (m_inWallJumpLock) return;

    // Probe slightly beyond edges to detect wall contact even at zero velocity
    const float probeOffset = 0.01f;

    Vec2 leftProbe = Vec2(position.x + m_playerBoundingBox[0].x - probeOffset, position.y);
    bool leftSolid = m_world->IsSolid(GetGrid()->GetCellFromGridPosition(leftProbe));

    Vec2 rightProbe = Vec2(position.x + m_playerBoundingBox[1].x + probeOffset, position.y);
    bool rightSolid = m_world->IsSolid(GetGrid()->GetCellFromGridPosition(rightProbe));

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
    m_isOnWall = onWall;
    m_wallDirection = direction;

    if (onWall)
    {
        ClearJumpState();
        ClearWallJumpTracking();

        // Execute buffered jump immediately on wall grab
        if (m_jumpCommandQueue.HasCommands())
        {
            auto cmd = m_jumpCommandQueue.DequeueCommand();
            if (cmd) cmd->Execute();
            m_jumpBufferTimer.Stop();
        }
    }
}

void Player::EnterWallJump()
{
    // Decompose wall jump force into horizontal and vertical components
    float angleRad = m_wallJumpAngle * (std::numbers::pi_v<float> / 180.0f);
    m_velocity.x = -m_wallDirection * m_wallJumpForce * std::cos(angleRad);
    m_velocity.y = -m_wallJumpForce * std::sin(angleRad);

    m_isJumping = true;
    m_inMinJump = true;
    m_minJumpTimer.Reset();

    // Lock phase: no player input for the duration of the timer
    m_inWallJumpLock = true;
    m_wallJumpCoasting = true;
    m_wallJumpLockTimer.Reset();

    ChangeWallState(false, 0);
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
}
