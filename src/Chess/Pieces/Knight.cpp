#include "Knight.h"

using namespace Engine;

void Knight::Render() const
{
    // Render the knight as a rectangle with an elongated "face" represented by another rectangle
    Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);
    Vec2 faceSize = Vec2(m_worldSize.x * 0.4f, m_worldSize.y * 0.3f);
    Vec2 facePosition = m_worldPosition + Vec2((m_worldSize.x + faceSize.x) / 2, (m_worldSize.y - faceSize.y) / 2);
    Renderer2D::DrawTile(facePosition, faceSize, m_color);
}

std::vector<Vec2> Knight::GetPossibleMoves() const
{
    return GetSteppingMoves({
        Vec2(2, 1), Vec2(2, -1), Vec2(-2, 1), Vec2(-2, -1),
        Vec2(1, 2), Vec2(1, -2), Vec2(-1, 2), Vec2(-1, -2)
    });
}
