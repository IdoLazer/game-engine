#pragma once
#include "ChessPiece.h"

class Knight : public ChessPiece
{
    DECLARE_TYPE(Knight, ChessPiece)

// --- Constructors & Destructors ---
public:
    Knight() = default;

// --- Lifecycle ---
public:
    void Render() const override;

// --- Accessors ---
public:
    std::vector<Engine::Vec2> GetPossibleMoves() const override;
};