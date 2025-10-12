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

    // If the pawn hasn't moved yet, it can move forward two squares
    if (!m_hasMoved)
    {
        Vec2 doubleForwardMove = m_gridPosition + (m_ChessPieceColor == ChessPieceColor::White ? Vec2(0, 2) : Vec2(0, -2));
        if (m_board->IsValidPosition(doubleForwardMove))
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

void Pawn::Render()
{
    // Render the pawn as a circle with a small rectangle base
    // First, render the base rectangle
    Vec2 baseSize = Vec2(m_worldSize.x * 0.8f, m_worldSize.y * 0.4f);
    Renderer2D::DrawTile(m_worldPosition - Vec2(0.0f, baseSize.y / 2.0f), baseSize, m_color);

    // Then, render the circular top
    float radius = m_worldSize.x * 0.3f;
    Renderer2D::DrawCircle(m_worldPosition + Vec2(0.0f, radius), radius, m_color);
}

void Pawn::OnMove(const Vec2 &newPosition)
{
    ChessPiece::OnMove(newPosition);

    m_hasMoved = true;
}
