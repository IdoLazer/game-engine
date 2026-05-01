#pragma once
#include "ChessPiece.h"

class Knight : public ChessPiece
{
    DECLARE_TYPE(Knight, ChessPiece)

// --- Constructors & Destructors ---
public:
    Knight() = default;

// --- Accessors ---
public:
    std::vector<Engine::Vec2> GetPossibleMoves() const override;
    std::string GetPieceName() const override { return "Knight"; }
};