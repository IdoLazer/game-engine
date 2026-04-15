#pragma once
#include "ChessPiece.h"

class Bishop : public ChessPiece
{
    DECLARE_TYPE(Bishop, ChessPiece)

// --- Constructors & Destructors ---
public:
    Bishop() = default;

// --- Lifecycle ---
public:
    void Render() const override;

// --- Accessors ---
public:
    std::vector<Engine::Vec2> GetPossibleMoves() const override;
};