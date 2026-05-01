#include "Bishop.h"

// --- Type Registration ---

BEGIN_TYPE_REGISTER(Bishop)
END_TYPE_REGISTER()

using namespace Engine;

// --- Accessors ---

std::vector<Vec2> Bishop::GetPossibleMoves() const
{
    return GetSlidingMoves({
        ChessConstants::DIRECTION_NE, ChessConstants::DIRECTION_SE,
        ChessConstants::DIRECTION_NW, ChessConstants::DIRECTION_SW
    });
}