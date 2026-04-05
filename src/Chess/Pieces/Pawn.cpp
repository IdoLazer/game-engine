#include "Pawn.h"

std::vector<Vec2> Pawn::GetPossibleMoves() const
{
    std::vector<Vec2> possibleMoves;

    // Pawns can move forward one square
    Vec2 forwardMove = m_gridPosition + (m_ChessPieceColor == ChessPieceColor::White ? Vec2(0, 1) : Vec2(0, -1));
    if (m_board->IsValidPosition(forwardMove))
    {
        possibleMoves.push_back(forwardMove);
    }

    // If the pawn hasn't moved yet, it can move forward two squares, but only if both squares are unoccupied
    if (!m_hasMoved)
    {
        Vec2 doubleForwardMove = m_gridPosition + (m_ChessPieceColor == ChessPieceColor::White ? Vec2(0, 2) : Vec2(0, -2));
        if (m_board->IsValidPosition(forwardMove) && m_board->IsValidPosition(doubleForwardMove))
        {
            possibleMoves.push_back(doubleForwardMove);
        }
    }

    // Pawns can capture diagonally
    Vec2 captureLeft = m_gridPosition + (m_ChessPieceColor == ChessPieceColor::White ? Vec2(-1, 1) : Vec2(-1, -1));
    Vec2 captureRight = m_gridPosition + (m_ChessPieceColor == ChessPieceColor::White ? Vec2(1, 1) : Vec2(1, -1));
    if (
        m_board->IsInBounds(captureLeft) &&
        m_board->IsOccupied(captureLeft) &&
        m_board->GetPieceAt(captureLeft)->GetPieceColor() != m_ChessPieceColor)
    {
        possibleMoves.push_back(captureLeft);
    }
    if (
        m_board->IsInBounds(captureRight) &&
        m_board->IsOccupied(captureRight) &&
        m_board->GetPieceAt(captureRight)->GetPieceColor() != m_ChessPieceColor)
    {
        possibleMoves.push_back(captureRight);
    }

    return possibleMoves;
}

void Pawn::Initialize()
{
    Entity::Initialize();

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

void Pawn::OnMove(const Vec2 &newPosition)
{
    ChessPiece::OnMove(newPosition);

    m_hasMoved = true;
}
