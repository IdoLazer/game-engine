#pragma once

#include "ChessPiece.h"

class Knight : public ChessPiece
{

public:
    Knight(const ChessBoard *board, const Vec2 &position, PieceColor color)
        : ChessPiece(board, position, ChessConstants::KNIGHT_SIZE, color) {}

    virtual void Render() const override;

    virtual std::vector<Vec2> GetPossibleMoves() const override;
};