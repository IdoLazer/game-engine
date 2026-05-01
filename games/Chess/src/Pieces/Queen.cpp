#include "Queen.h"

// --- Type Registration ---

BEGIN_TYPE_REGISTER(Queen)
END_TYPE_REGISTER()

using namespace Engine;

// --- Accessors ---

std::vector<Vec2> Queen::GetPossibleMoves() const
{
    return GetSlidingMoves({
        ChessConstants::DIRECTION_E, ChessConstants::DIRECTION_W,
        ChessConstants::DIRECTION_N, ChessConstants::DIRECTION_S,
        ChessConstants::DIRECTION_NE, ChessConstants::DIRECTION_SE,
        ChessConstants::DIRECTION_NW, ChessConstants::DIRECTION_SW
    });
}
