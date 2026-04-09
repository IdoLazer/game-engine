#include "Queen.h"

using namespace Engine;

void Queen::Render() const
{
    // Render the queen as an elongated triangle, a narrow rectangle on top,
    // an upside-down triangle on top of that, and a small circle at the very top

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

    // Finally, render the small circle at the very top of the queen
    float circleRadius = m_worldSize.x / 12.0f;
    Vec2 circleCenter = invertedTopCenter + Vec2(0.0f, m_worldSize.y / 3.0f + circleRadius);
    Renderer2D::DrawCircle(circleCenter, circleRadius, m_color);
}

std::vector<Vec2> Queen::GetPossibleMoves() const
{
    return GetSlidingMoves({
        ChessConstants::DIRECTION_E, ChessConstants::DIRECTION_W,
        ChessConstants::DIRECTION_N, ChessConstants::DIRECTION_S,
        ChessConstants::DIRECTION_NE, ChessConstants::DIRECTION_SE,
        ChessConstants::DIRECTION_NW, ChessConstants::DIRECTION_SW
    });
}
