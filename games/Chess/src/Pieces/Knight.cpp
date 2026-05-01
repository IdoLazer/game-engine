#include "Knight.h"

// --- Type Registration ---

BEGIN_TYPE_REGISTER(Knight)
END_TYPE_REGISTER()

using namespace Engine;

// --- Accessors ---

std::vector<Vec2> Knight::GetPossibleMoves() const
{
    return GetSteppingMoves({
        Vec2(2, 1), Vec2(2, -1), Vec2(-2, 1), Vec2(-2, -1),
        Vec2(1, 2), Vec2(1, -2), Vec2(-1, 2), Vec2(-1, -2)
    });
}
