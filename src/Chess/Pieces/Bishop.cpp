#include "Bishop.h"

void Bishop::Render() const
{
    // Draw the bishop as an elongated triangle, a rectangle on top, 
    // then a circle on top of that with a smaller circle on top of the larger circle

    // First, render the elongated triangle base
    Vec2 topCenter = m_worldPosition;
    Vec2 leftBase = m_worldPosition + Vec2(-m_worldSize.x / 5.0f, -5.0f * m_worldSize.y / 12.0f);
    Vec2 rightBase = m_worldPosition + Vec2(m_worldSize.x / 5.0f, -5.0f * m_worldSize.y / 12.0f);
    Renderer2D::DrawTriangle(topCenter, leftBase, rightBase, m_color);

    // Then, render the rectangle on top of the triangle
    Vec2 rectSize = Vec2(m_worldSize.x / 3.0f, m_worldSize.y / 10.0f);
    Vec2 rectPos = topCenter;
    Renderer2D::DrawTile(rectPos, rectSize, m_color);

    float largeRadius = m_worldSize.x / 8.0f;
    Vec2 circleCenter = rectPos + Vec2(0.0f, rectSize.y / 2.0f + largeRadius);
    Renderer2D::DrawCircle(circleCenter, largeRadius, m_color);
    
    // Finally, render the smaller circle on top of the larger circle
    float smallRadius = m_worldSize.x / 12.0f;
    Vec2 smallCircleCenter = circleCenter + Vec2(0.0f, largeRadius + smallRadius / 2.0f);
    Renderer2D::DrawCircle(smallCircleCenter, smallRadius, m_color);
}

std::vector<Vec2> Bishop::GetPossibleMoves() const
{
    std::vector<Vec2> possibleMoves;
    // Bishop moves diagonally in all four directions
    std::vector<Vec2> directions = {Vec2(1, 1), Vec2(1, -1), Vec2(-1, 1), Vec2(-1, -1)};

    for (const Vec2 &dir : directions)
    {
        Vec2 currentPos = m_gridPosition + dir;
        while (m_board->IsValidPosition(currentPos))
        {
            possibleMoves.push_back(currentPos);
            currentPos += dir;
        }
        if (m_board->IsOccupied(currentPos))
        {
            ChessPiece *pieceAtPos = m_board->GetPieceAt(currentPos);
            if (pieceAtPos->GetPieceColor() != m_ChessPieceColor)
            {
                possibleMoves.push_back(currentPos); // Can capture opponent's piece
            }
        }
    }

    return possibleMoves;
}