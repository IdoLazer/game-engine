#include "ChessPiece.h"
#include "../ChessBoard.h"

// --- Type Registration ---

BEGIN_TYPE_REGISTER(ChessPiece)
    REGISTER_PROPERTY(PieceColor, PieceColor, &ChessPiece::m_pieceColor)
END_TYPE_REGISTER()

using namespace Engine;

// --- Lifecycle ---

void ChessPiece::Initialize()
{
    // Derive visual color from piece color
    m_color = (m_pieceColor == PieceColor::White) ? ChessConstants::PIECE_COLOR_WHITE : ChessConstants::PIECE_COLOR_BLACK;

    // Register with grid (GridEntity::Initialize handles sync + registration)
    GridEntity::Initialize();
}

// --- Accessors ---

PieceColor ChessPiece::GetPieceColor() const { return m_pieceColor; }

// --- Internal ---

bool ChessPiece::IsCellEmpty(const Vec2 &pos) const
{
    return m_grid->GetFirstEntityAt<ChessPiece>(pos) == nullptr;
}

bool ChessPiece::IsCellValidAndEmpty(const Vec2 &pos) const
{
    return m_grid->IsInBounds(pos) && IsCellEmpty(pos);
}

ChessPiece *ChessPiece::GetPieceAt(const Vec2 &pos) const
{
    return m_grid->GetFirstEntityAt<ChessPiece>(pos);
}

// --- Movement helpers ---

std::vector<Vec2> ChessPiece::GetSlidingMoves(const std::vector<Vec2> &directions) const
{
    std::vector<Vec2> moves;
    for (const Vec2 &dir : directions)
    {
        Vec2 pos = m_gridPosition + dir;
        while (IsCellValidAndEmpty(pos))
        {
            moves.push_back(pos);
            pos += dir;
        }
        // Check if the blocking piece is capturable
        if (m_grid->IsInBounds(pos))
        {
            ChessPiece *blocker = GetPieceAt(pos);
            if (blocker && blocker->GetPieceColor() != m_pieceColor)
                moves.push_back(pos);
        }
    }
    return moves;
}

std::vector<Vec2> ChessPiece::GetSteppingMoves(const std::vector<Vec2> &directions) const
{
    std::vector<Vec2> moves;
    for (const Vec2 &dir : directions)
    {
        Vec2 pos = m_gridPosition + dir;
        if (IsCellValidAndEmpty(pos))
        {
            moves.push_back(pos);
        }
        else if (m_grid->IsInBounds(pos))
        {
            ChessPiece *target = GetPieceAt(pos);
            if (target && target->GetPieceColor() != m_pieceColor)
                moves.push_back(pos);
        }
    }
    return moves;
}
