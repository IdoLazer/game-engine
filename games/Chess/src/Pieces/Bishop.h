#pragma once
#include "ChessPiece.h"

class Bishop : public ChessPiece
{
    DECLARE_TYPE(Bishop, ChessPiece)

// --- Constructors & Destructors ---
public:
    Bishop() = default;

// --- Accessors ---
public:
    std::vector<Engine::Vec2> GetPossibleMoves() const override;
    std::string GetPieceName() const override { return "Bishop"; }
};