#include "Player.h"

void Player::Initialize()
{
    // Set up movement timer
    m_moveTimer = Timer{1.0f / m_moveSpeed, [this]()
                        { Move(); }, true};

    // Create initial tail segments
    for (int i = 0; i < m_tailSegments.capacity(); ++i)
    {
        Vec2 tailPosition = Vec2(m_gridPosition.x - (i + 1) * m_direction.x, m_gridPosition.y - (i + 1) * m_direction.y);
        GridTile gridTile(m_grid, tailPosition, m_gridSize, m_color);
        m_tailSegments.emplace_back(gridTile);
    }
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
    // Draw Player head using inherited world coordinates (automatically synced from grid position)
    Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);

    // Draw tail segments
    for (const GridTile &segment : m_tailSegments)
    {
        segment.Render();
    }
}

void Player::Move()
{
    if (m_growTailOnNextMove)
    {
        m_tailSegments.push_back(GridTile(m_grid, m_gridPosition, m_gridSize, m_color));
        m_growTailOnNextMove = false;
    }
    // Move the tail segments by shifting them forward and placing the previous head position at the front
    if (!m_tailSegments.empty())
    {
        std::rotate(m_tailSegments.rbegin(), m_tailSegments.rbegin() + 1, m_tailSegments.rend());
        m_tailSegments.front().SetGridPosition(m_gridPosition);
    }

    // Update grid position (this automatically updates world position via inheritance)
    SetGridPosition(m_gridPosition + m_direction);
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
    for (const GridTile &segment : m_tailSegments)
    {
        if (m_gridPosition == segment.GetGridPosition())
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
