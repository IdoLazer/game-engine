#include "Pawn.h"

using namespace Engine;

std::vector<Vec2> Pawn::GetPossibleMoves() const
{
    std::vector<Vec2> possibleMoves;

    Vec2 forward = (m_pieceColor == PieceColor::White) ? ChessConstants::DIRECTION_N : ChessConstants::DIRECTION_S;

    // Pawns can move forward one square
    Vec2 forwardMove = m_gridPosition + forward;
    if (IsCellValidAndEmpty(forwardMove))
    {
        possibleMoves.push_back(forwardMove);
    }

    // If the pawn hasn't moved yet, it can move forward two squares, but only if both squares are unoccupied
    if (!m_hasMoved)
    {
        Vec2 doubleForwardMove = m_gridPosition + forward + forward;
        if (IsCellValidAndEmpty(forwardMove) && IsCellValidAndEmpty(doubleForwardMove))
        {
            possibleMoves.push_back(doubleForwardMove);
        }
    }

    // Pawns can capture diagonally
    Vec2 captureLeft = m_gridPosition + forward + ChessConstants::DIRECTION_W;
    Vec2 captureRight = m_gridPosition + forward + ChessConstants::DIRECTION_E;
    if (m_grid->IsInBounds(captureLeft))
    {
        ChessPiece *target = GetPieceAt(captureLeft);
        if (target && target->GetPieceColor() != m_pieceColor)
            possibleMoves.push_back(captureLeft);
    }
    if (m_grid->IsInBounds(captureRight))
    {
        ChessPiece *target = GetPieceAt(captureRight);
        if (target && target->GetPieceColor() != m_pieceColor)
            possibleMoves.push_back(captureRight);
    }

    return possibleMoves;
}

void Pawn::Initialize()
{
    GridEntity::Initialize();

    m_hasMoved = false;
}

void Pawn::Render() const
{
    // Draw the pawn as a triangle with a circle on top
    Vec2 topCenter = m_worldPosition + Vec2(0.0f, m_worldSize.y / 4.0f);
    Vec2 leftBase = m_worldPosition + Vec2(-m_worldSize.x / 5.0f, -m_worldSize.y / 4.0f);
    Vec2 rightBase = m_worldPosition + Vec2(m_worldSize.x / 5.0f, -m_worldSize.y / 4.0f);
    Renderer2D::DrawTriangle(topCenter, leftBase, rightBase, m_color);
    // Draw the circle on top of the triangle
    Renderer2D::DrawCircle(topCenter, m_worldSize.x / 7.0f, m_color);
}

void Pawn::SetGridPosition(const Vec2 &newPosition)
{
    ChessPiece::SetGridPosition(newPosition);

    m_hasMoved = true;
}
