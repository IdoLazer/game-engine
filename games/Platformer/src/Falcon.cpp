#include "Falcon.h"
#include "Player.h"
#include "Cursor.h"

using namespace Engine;

BEGIN_TYPE_REGISTER(Falcon)
    REGISTER_PROPERTY(Engine::Vec2, OffsetFromPlayer, &Falcon::m_offsetFromPlayer)
END_TYPE_REGISTER()

void Falcon::Initialize()
{
    GridEntity::Initialize();

    // Set the falcon's initial position relative to the player
    if (m_player)
    {
        SetGridPosition(m_player->GetGridPosition() + m_offsetFromPlayer);
    }
}

void Falcon::Update(float deltaTime)
{
    if (m_isFollowingPlayer && m_player)
    {
        // Update the falcon's position to follow the player with the specified offset
        SetGridPosition(m_player->GetGridPosition() + m_offsetFromPlayer);
    }
    if (m_isAiming)
    {
        // If aiming, set the goal to the cursor's position
        SetGoal(m_grid->WorldToGrid(m_cursor ? m_cursor->GetWorldPosition() : GetWorldPosition()));
    }
}

void Falcon::Render() const
{
    // Convert the falcon's grid position and size to world coordinates for rendering
    Vec2 worldCenter = GetWorldPosition();
    Vec2 worldSize = GetWorldSize();
    Vec2 worldGoal = m_grid->GridToWorld(m_goal);
    Vec2 worldDirection = Vec2(m_direction.x, -m_direction.y).Normalized(); // Invert y for rendering
    
    // Draw falcon as a white triangle pointing in the direction it's facing

    // Falcon's "face" - the point in the direction it's facing
    Vec2 p1 = worldCenter + worldDirection * (worldSize.x / 2.0f);
    
    // The two other points are in the opposite direction, forming a triangle
    Vec2 perp = Vec2(-worldDirection.y, worldDirection.x).Normalized(); // Perpendicular vector to the direction
    Vec2 p2 = worldCenter - worldDirection * (worldSize.x / 2.0f) + perp * (worldSize.y / 2.0f);
    Vec2 p3 = worldCenter - worldDirection * (worldSize.x / 2.0f) - perp * (worldSize.y / 2.0f);

    Renderer2D::DrawTriangle(p1, p2, p3, m_color);

    if (m_isAiming)
    {
        // Draw a line from the falcon to its goal position for debugging purposes
        Renderer2D::DrawLine(worldCenter, worldGoal, Color::White, 0.02f, LineStyle::Dashed);
    }
}

void Falcon::SetGoal(const Engine::Vec2 &goal)
{
    m_goal = goal;
    m_direction = (m_goal - GetGridPosition()).Normalized();
}
