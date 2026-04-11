#pragma once
#include <Engine.h>
#include "ChessPiece.h"

class Pawn : public ChessPiece
{
private:
    bool m_hasMoved{false};

public:
    Pawn(Engine::Grid *grid, const Engine::Vec2 &position, PieceColor color)
        : ChessPiece(grid, position, ChessConstants::PAWN_SIZE, color) {}

    virtual void Initialize() override;

    virtual void Render() const override;

    virtual std::vector<Engine::Vec2> GetPossibleMoves() const override;

    void SetGridPosition(const Engine::Vec2 &newPosition) override;
};