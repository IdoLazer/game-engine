#include "Player.h"

void Player::Initialize(Grid *grid, const Vec2 &startPos, const Vec2 &size, const Color &color, const Vec2 &startDir, int initialTailLength, float moveSpeed)
{
    m_grid = grid;
    m_position = startPos;
    m_size = size;
    m_color = color;

    m_direction = startDir;

    // Create initial tail segments
    m_tailSegments = std::vector<Vec2>(initialTailLength, startPos);
    for (int i = 0; i < initialTailLength; ++i)
    {
        m_tailSegments[i] = Vec2(startPos.x - (i + 1) * startDir.x, startPos.y - (i + 1) * startDir.y);
    }

    m_moveSpeed = moveSpeed;
    m_moveTimer = Timer{1.0f / m_moveSpeed, [this]()
                        { Move(); }, true};
}

void Player::Destroy()
{
    m_tailSegments.clear();
}

void Player::Update(float deltaTime)
{
    m_moveTimer.Update(deltaTime);
}

void Player::Render()
{
    // Draw Player - use Grid to convert grid coordinates to world positions
    Renderer2D::DrawTile(m_grid->GridToWorld(m_position), m_grid->GetCellSize() * m_size, m_color);
    for (const Vec2 &segment : m_tailSegments)
    {
        Renderer2D::DrawTile(m_grid->GridToWorld(segment), m_grid->GetCellSize() * m_size, m_color);
    }
}

void Player::Move()
{
    if (m_growTailOnNextMove)
    {
        m_tailSegments.push_back(m_position);
        m_growTailOnNextMove = false;
    }
    // Move the tail segments by shifting them forward and placing the previous head position at the front
    if (!m_tailSegments.empty())
    {
        std::rotate(m_tailSegments.rbegin(), m_tailSegments.rbegin() + 1, m_tailSegments.rend());
        m_tailSegments.front() = m_position;
    }
    m_position += m_direction;
    m_updateMoveThisFrame = false;
}

void Player::Grow()
{
    m_growTailOnNextMove = true;
}

bool Player::CheckSelfCollision() const
{
    // Check if the player has collided with its own tail
    bool isColliding = false;
    for (const Vec2 &segment : m_tailSegments)
    {
        if (m_position == segment)
        {
            isColliding = true;
            break;
        }
    }
    return isColliding;
}

void Player::SetDirection(const Vec2 &dir)
{
    if (m_updateMoveThisFrame)
        return;
    m_updateMoveThisFrame = true;
    m_direction = dir;
}
