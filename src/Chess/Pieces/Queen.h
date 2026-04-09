#pragma once

#include "ChessPiece.h"

class Queen : public ChessPiece
{

public:
    Queen(const ChessBoard *board, const Vec2 &position, PieceColor color)
        : ChessPiece(board, position, ChessConstants::QUEEN_SIZE, color) {}

    virtual void Render() const override;

    virtual std::vector<Vec2> GetPossibleMoves() const override;
};