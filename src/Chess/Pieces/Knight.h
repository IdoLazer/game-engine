#pragma once

#include "ChessPiece.h"

class Knight : public ChessPiece
{

public:
    Knight(const ChessBoard *board, const Vec2 &position, ChessPieceColor color)
        : ChessPiece(board, position, Vec2(0.3f, 0.6f), color) {}

    virtual void Render() override;

    virtual std::vector<Vec2> GetPossibleMoves() const override;
};