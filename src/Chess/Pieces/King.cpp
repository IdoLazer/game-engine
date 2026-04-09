#include "King.h"

using namespace Engine;

void King::Render() const
{
    // Render the king as an elongated triangle, a narrow rectangle on top,
    // an upside-down triangle on top of that, and a small cross at the very top
    // First, render the elongated triangle base
    Vec2 topCenter = m_worldPosition;
    Vec2 leftBase = m_worldPosition + Vec2(-m_worldSize.x / 5.0f, -5.0f * m_worldSize.y / 12.0f);
    Vec2 rightBase = m_worldPosition + Vec2(m_worldSize.x / 5.0f, -5.0f * m_worldSize.y / 12.0f);
    Renderer2D::DrawTriangle(topCenter, leftBase, rightBase, m_color);

    // Then, render the rectangle on top of the triangle
    Vec2 rectSize = Vec2(m_worldSize.x / 3.0f, m_worldSize.y / 10.0f);
    Vec2 rectPos = topCenter;
    Renderer2D::DrawTile(rectPos, rectSize, m_color);

    // Next, render the upside-down triangle on top of the rectangle
    Vec2 invertedTopCenter = rectPos - Vec2(0.0f, rectSize.y / 2.0f);
    Vec2 invertedLeftBase = invertedTopCenter + Vec2(-m_worldSize.x / 5.0f, m_worldSize.y / 3.0f);
    Vec2 invertedRightBase = invertedTopCenter + Vec2(m_worldSize.x / 5.0f, m_worldSize.y / 3.0f);
    Renderer2D::DrawTriangle(invertedTopCenter, invertedLeftBase, invertedRightBase, m_color);

    // Finally, render the small cross at the very top of the king
    float crossSize = m_worldSize.x / 6.0f;
    Vec2 crossCenter = invertedTopCenter + Vec2(0.0f, m_worldSize.y / 3.0f + crossSize / 2.0f);
    Renderer2D::DrawTile(crossCenter, Vec2(crossSize / 4.0f, crossSize), m_color); // Vertical part of the cross
    Renderer2D::DrawTile(crossCenter, Vec2(crossSize, crossSize / 4.0f), m_color); // Horizontal part of the cross
}

std::vector<Vec2> King::GetPossibleMoves() const
{
    return GetSteppingMoves({
        ChessConstants::DIRECTION_E, ChessConstants::DIRECTION_W,
        ChessConstants::DIRECTION_N, ChessConstants::DIRECTION_S,
        ChessConstants::DIRECTION_NE, ChessConstants::DIRECTION_SE,
        ChessConstants::DIRECTION_NW, ChessConstants::DIRECTION_SW
    });
}