#pragma once
#include "ChessPiece.h"

class Queen : public ChessPiece
{
    DECLARE_TYPE(Queen, ChessPiece)

// --- Constructors & Destructors ---
public:
    Queen() = default;

// --- Lifecycle ---
public:
    void Render() const override;

// --- Accessors ---
public:
    std::vector<Engine::Vec2> GetPossibleMoves() const override;
};