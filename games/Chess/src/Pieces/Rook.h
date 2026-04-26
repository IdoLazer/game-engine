#pragma once
#include "ChessPiece.h"

class Rook : public ChessPiece
{
    DECLARE_TYPE(Rook, ChessPiece)

// --- Fields ---
private:
    bool m_hasMoved{false};

// --- Constructors & Destructors ---
public:
    Rook() = default;

// --- Lifecycle ---
public:
    void Initialize() override;
    void Render() const override;

// --- Accessors ---
public:
    std::vector<Engine::Vec2> GetPossibleMoves() const override;
    void SetGridPosition(const Engine::Vec2 &newPosition) override;
    bool HasMoved() const;
};