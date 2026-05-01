#pragma once
#include "ChessPiece.h"

class Queen : public ChessPiece
{
    DECLARE_TYPE(Queen, ChessPiece)

// --- Constructors & Destructors ---
public:
    Queen() = default;

// --- Accessors ---
public:
    std::vector<Engine::Vec2> GetPossibleMoves() const override;
    std::string GetPieceName() const override { return "Queen"; }
};