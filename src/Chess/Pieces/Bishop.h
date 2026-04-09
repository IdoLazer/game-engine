#pragma once

#include "ChessPiece.h"

class Bishop : public ChessPiece
{

public:
    Bishop(const ChessBoard *board, const Vec2 &position, PieceColor color)
        : ChessPiece(board, position, ChessConstants::BISHOP_SIZE, color) {}

    virtual void Render() const override;

    virtual std::vector<Vec2> GetPossibleMoves() const override;
};