#include "Knight.h"

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
    std::vector<Vec2> moves;

    // Knight moves in an L-shape: 2 squares in one direction and 1 square perpendicular
    std::vector<Vec2> directions = {
        Vec2(2, 1), Vec2(2, -1), Vec2(-2, 1), Vec2(-2, -1),
        Vec2(1, 2), Vec2(1, -2), Vec2(-1, 2), Vec2(-1, -2)};

    for (const Vec2 &dir : directions)
    {
        Vec2 newPos = m_gridPosition + dir;
        if (m_board->IsValidPosition(newPos))
        {
            moves.push_back(newPos);
        }
        else if (m_board->IsOccupied(newPos))
        {
            ChessPiece *targetPiece = m_board->GetPieceAt(newPos);
            if (targetPiece && targetPiece->GetPieceColor() != m_ChessPieceColor)
            {
                moves.push_back(newPos); // Can capture opponent's piece
            }
        }
    }

    return moves;
}
