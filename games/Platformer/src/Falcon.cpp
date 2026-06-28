#include "Falcon.h"
#include "Player.h"

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
}

void Falcon::Render() const
{
    // Draw falcon as a white triangle pointing in the direction it's facing
    Vec2 center = GetWorldPosition();
    Vec2 size = GetWorldSize();

    // Falcon's "face" - the point in the direction it's facing
    Vec2 p1 = center + m_direction.Normalized() * (size.x / 2.0f);
    
    // The two other points are in the opposite direction, forming a triangle
    Vec2 perp = Vec2(-m_direction.y, m_direction.x).Normalized(); // Perpendicular vector to the direction
    Vec2 p2 = center - m_direction.Normalized() * (size.x / 2.0f) + perp * (size.y / 2.0f);
    Vec2 p3 = center - m_direction.Normalized() * (size.x / 2.0f) - perp * (size.y / 2.0f);

    Renderer2D::DrawTriangle(p1, p2, p3, m_color);
}
