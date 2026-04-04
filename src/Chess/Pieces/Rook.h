#pragma once

#include "ChessPiece.h"

class Rook : public ChessPiece
{
private:
    bool m_hasMoved;

public:
    Rook(const ChessBoard *board, const Vec2 &position, ChessPieceColor color)
        : ChessPiece(board, position, Vec2(0.4f, 0.6f), color) {}

    virtual void Initialize() override;

    virtual void Render() const override;

    virtual std::vector<Vec2> GetPossibleMoves() const override;

    virtual void OnMove(const Vec2 &newPosition) override;
};