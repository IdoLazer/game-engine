#pragma once
#include <Engine.h>
#include "ChessPiece.h"

class Pawn : public ChessPiece
{
private:
    bool m_hasMoved{false};

public:
    Pawn(const ChessBoard *board, const Vec2 &position, ChessPieceColor color)
        : ChessPiece(board, position, Vec2(0.5f, 0.5f), color) {}

    virtual void Initialize() override;

    virtual void Render() const override;

    virtual std::vector<Vec2> GetPossibleMoves() const override;

    virtual void OnMove(const Vec2 &newPosition) override;
};