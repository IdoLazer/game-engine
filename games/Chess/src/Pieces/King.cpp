#include "King.h"

// --- Type Registration ---

BEGIN_TYPE_REGISTER(King)
END_TYPE_REGISTER()

using namespace Engine;

// --- Lifecycle ---

void King::Initialize()
{
    m_hasMoved = false;
    ChessPiece::Initialize();
}

// --- Accessors ---

std::vector<Vec2> King::GetPossibleMoves() const
{
    return GetSteppingMoves({
        ChessConstants::DIRECTION_E, ChessConstants::DIRECTION_W,
        ChessConstants::DIRECTION_N, ChessConstants::DIRECTION_S,
        ChessConstants::DIRECTION_NE, ChessConstants::DIRECTION_SE,
        ChessConstants::DIRECTION_NW, ChessConstants::DIRECTION_SW
    });
}

void King::SetGridPosition(const Vec2 &newPosition)
{
    ChessPiece::SetGridPosition(newPosition);
    m_hasMoved = true;
}

bool King::HasMoved() const { return m_hasMoved; }