#pragma once

#include "ChessPiece.h"

class King : public ChessPiece
{

public:
    King(const ChessBoard *board, const Vec2 &position, PieceColor color)
        : ChessPiece(board, position, ChessConstants::KING_SIZE, color) {}

    virtual void Render() const override;

    virtual std::vector<Vec2> GetPossibleMoves() const override;
};