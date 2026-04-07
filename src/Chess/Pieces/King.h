#pragma once

#include "ChessPiece.h"

class King : public ChessPiece
{

public:
    King(const ChessBoard *board, const Vec2 &position, ChessPieceColor color)
        : ChessPiece(board, position, Vec2(1.0f, 1.0f), color) {}

    virtual void Render() const override;

    virtual std::vector<Vec2> GetPossibleMoves() const override;
};