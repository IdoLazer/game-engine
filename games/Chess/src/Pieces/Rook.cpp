#include "Rook.h"

// --- Type Registration ---

BEGIN_TYPE_REGISTER(Rook)
END_TYPE_REGISTER()

using namespace Engine;

// --- Lifecycle ---

void Rook::Initialize()
{
    m_hasMoved = false;
    ChessPiece::Initialize();
}

// --- Accessors ---

std::vector<Vec2> Rook::GetPossibleMoves() const
{
    return GetSlidingMoves({
        ChessConstants::DIRECTION_E, ChessConstants::DIRECTION_W,
        ChessConstants::DIRECTION_N, ChessConstants::DIRECTION_S
    });
}

void Rook::SetGridPosition(const Vec2 &newPosition)
{
    ChessPiece::SetGridPosition(newPosition);
    m_hasMoved = true;
}

bool Rook::HasMoved() const { return m_hasMoved; }
