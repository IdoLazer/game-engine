#pragma once

#include "ChessPiece.h"

class Rook : public ChessPiece
{
private:
    bool m_hasMoved;

public:
    Rook(Engine::Grid *grid, const Engine::Vec2 &position, PieceColor color)
        : ChessPiece(grid, position, ChessConstants::ROOK_SIZE, color) {}

    virtual void Initialize() override;

    virtual void Render() const override;

    virtual std::vector<Engine::Vec2> GetPossibleMoves() const override;

    virtual void SetGridPosition(const Engine::Vec2 &newPosition) override;
};