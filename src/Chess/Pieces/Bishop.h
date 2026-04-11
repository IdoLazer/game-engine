#pragma once

#include "ChessPiece.h"

class Bishop : public ChessPiece
{

public:
    Bishop(Engine::Grid *grid, const Engine::Vec2 &position, PieceColor color)
        : ChessPiece(grid, position, ChessConstants::BISHOP_SIZE, color) {}

    virtual void Render() const override;

    virtual std::vector<Engine::Vec2> GetPossibleMoves() const override;
};