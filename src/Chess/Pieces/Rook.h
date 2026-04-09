#pragma once

#include "ChessPiece.h"

class Rook : public ChessPiece
{
private:
    bool m_hasMoved;

public:
    Rook(const ChessBoard *board, const Vec2 &position, PieceColor color)
        : ChessPiece(board, position, ChessConstants::ROOK_SIZE, color) {}

    virtual void Initialize() override;

    virtual void Render() const override;

    virtual std::vector<Vec2> GetPossibleMoves() const override;

    virtual void OnMove(const Vec2 &newPosition) override;
};