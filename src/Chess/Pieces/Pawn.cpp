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

    return possibleMoves;
}

void Pawn::Initialize()
{
    Entity::Initialize();

    m_hasMoved = false;
}

void Pawn::OnMove(const Vec2 &newPosition)
{
    ChessPiece::OnMove(newPosition);

    m_hasMoved = true;
}
