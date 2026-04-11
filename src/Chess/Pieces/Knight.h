#pragma once

#include "ChessPiece.h"

class Knight : public ChessPiece
{

public:
    Knight(Engine::Grid *grid, const Engine::Vec2 &position, PieceColor color)
        : ChessPiece(grid, position, ChessConstants::KNIGHT_SIZE, color) {}

    virtual void Render() const override;

    virtual std::vector<Engine::Vec2> GetPossibleMoves() const override;
};