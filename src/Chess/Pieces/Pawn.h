#pragma once
#include "ChessPiece.h"

class Pawn : public ChessPiece
{
    DECLARE_TYPE(Pawn, ChessPiece)

// --- Fields ---
private:
    bool m_hasMoved{false};

// --- Constructors & Destructors ---
public:
    Pawn() = default;

// --- Lifecycle ---
public:
    void Initialize() override;
    void Render() const override;

// --- Accessors ---
public:
    std::vector<Engine::Vec2> GetPossibleMoves() const override;
    void SetGridPosition(const Engine::Vec2 &newPosition) override;
};