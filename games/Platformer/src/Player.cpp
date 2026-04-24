#include "Player.h"
#include "PlatformTile.h"
#include "PlatformerInputManager.h"
#include <iostream>

// --- Type Registration ---
BEGIN_TYPE_REGISTER(Player)
    REGISTER_PROPERTY(float, Speed, &Player::m_speed)
    REGISTER_PROPERTY(float, JumpForce, &Player::m_jumpForce)
    REGISTER_PROPERTY(float, Gravity, &Player::m_gravity)
    REGISTER_PROPERTY(float, CoyoteTime, &Player::m_coyoteTime)
    REGISTER_PROPERTY(float, JumpBufferTime, &Player::m_jumpBufferTime)
    REGISTER_PROPERTY(float, MinJumpTime, &Player::m_minJumpTime)
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
}

void Player::Update(float deltaTime)
{
    m_coyoteTimer.Update(deltaTime);
    m_jumpBufferTimer.Update(deltaTime);
    m_minJumpTimer.Update(deltaTime);
    ApplyGravity(deltaTime);
    m_velocity.x = m_direction.x * m_speed;
    if (m_isJumping && m_velocity.y > 0) m_isJumping = false; // If we start falling, we're no longer in the jump state
    HandleCollisions(deltaTime);
}

void Player::Render() const
{
    Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);

    // Debug: Draw bounding box
    Vec2 center = GetWorldPosition();
    Vec2 topLeft = center + m_playerBoundingBox[0];
    Vec2 bottomRight = center + m_playerBoundingBox[1];
    Vec2 size = bottomRight - topLeft;
    Renderer2D::DrawRectOutline(center, size, Color(1.0f, 1.0f, 1.0f, 1.0f));
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

bool Player::IsJumping() const
{
    return m_isJumping;
}

void Player::Jump()
{
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
    else
    {
        // If we can't jump now, buffer the jump input in case we land soon
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
            // If we're still in the minimum jump time, we buffer the stop jump command to execute right after the min jump time expires
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

// --- Internal Methods ---

void Player::ApplyGravity(float deltaTime)
{
    m_velocity.y += m_gravity * deltaTime;
}

void Player::HandleCollisions(float deltaTime)
{
    // Check for collisions in all directions and set velocity accordingly
    
    Vec2 currentPos = GetGridPosition();
    Vec2 newGridPos = currentPos + m_velocity * deltaTime;

    // Check left collision
    if (m_velocity.x < 0)
    {
        Vec2 playerLeft = Vec2(newGridPos.x + m_playerBoundingBox[0].x, currentPos.y);
        auto cell = GetGrid()->GetCellFromGridPosition(playerLeft);
        auto platformTile = GetGrid()->GetFirstEntityAt<PlatformTile>(cell);
        if (platformTile)
        {
            // Stop horizontal movement
            m_velocity.x = 0;

            // Snap to edge of platform
            Vec2 platformCenter = platformTile->GetGridPosition();
            Vec2 platformRight = Vec2(platformCenter.x + platformTile->GetGridSize().x / 2.0f, platformCenter.y);
            newGridPos.x = platformRight.x - m_playerBoundingBox[0].x;
        }
    }
    // Check right collision
    else if (m_velocity.x > 0)
    {
        Vec2 playerRight = Vec2(newGridPos.x + m_playerBoundingBox[1].x, currentPos.y);
        auto cell = GetGrid()->GetCellFromGridPosition(playerRight);
        auto platformTile = GetGrid()->GetFirstEntityAt<PlatformTile>(cell);
        if (platformTile)
        {
            // Stop horizontal movement
            m_velocity.x = 0;

            // Snap to edge of platform
            Vec2 platformCenter = platformTile->GetGridPosition();
            Vec2 platformLeft = Vec2(platformCenter.x - platformTile->GetGridSize().x / 2.0f, platformCenter.y);
            newGridPos.x = platformLeft.x - m_playerBoundingBox[1].x;
        }
    }
    // Check upward collision
    if (m_velocity.y < 0)
    {
        Vec2 playerTop = Vec2(currentPos.x, newGridPos.y + m_playerBoundingBox[0].y);
        auto cell = GetGrid()->GetCellFromGridPosition(playerTop);
        auto platformTile = GetGrid()->GetFirstEntityAt<PlatformTile>(cell);
        if (platformTile)
        {
            // Stop upward movement
            m_velocity.y = 0;

            // Snap to edge of platform
            Vec2 platformCenter = platformTile->GetGridPosition();
            Vec2 platformBottom = Vec2(platformCenter.x, platformCenter.y + platformTile->GetGridSize().y / 2.0f);
            newGridPos.y = platformBottom.y - m_playerBoundingBox[0].y;
        }
    }
    // Finally, check downward collision and set grounded state
    else if (m_velocity.y >= 0)
    {
        Vec2 playerBottom = Vec2(currentPos.x, newGridPos.y + m_playerBoundingBox[1].y);
        auto nextCell = GetGrid()->GetCellFromGridPosition(playerBottom);
        auto platformTile = GetGrid()->GetFirstEntityAt<PlatformTile>(nextCell);
        if (platformTile)
        {
            // Stop downward movement
            m_velocity.y = 0;

            // Land on platform
            Vec2 platformCenter = platformTile->GetGridPosition();
            Vec2 platformTop = Vec2(platformCenter.x, platformCenter.y - platformTile->GetGridSize().y / 2.0f);
            newGridPos.y = platformTop.y - m_playerBoundingBox[1].y;
            ChangeGroundedState(true);
        }
        else
        {
            ChangeGroundedState(false);
        }
    }

    SetGridPosition(newGridPos);
}

void Player::ChangeGroundedState(bool grounded)
{
    if (m_isGrounded == grounded) return; // No change
    m_isGrounded = grounded;
    if (grounded)
    {
        m_isJumping = false;
        m_coyoteTimer.Stop();
        m_inCoyoteTime = false;
        // Check if we have a buffered jump input and execute it immediately if we do
        if (m_jumpCommandQueue.HasCommands())
        {
            auto cmd = m_jumpCommandQueue.DequeueCommand();
            if (cmd) cmd->Execute();
            m_jumpBufferTimer.Stop();
        }
    }
    else
    {
        m_inCoyoteTime = true;
        m_coyoteTimer.Reset();
    }
}
