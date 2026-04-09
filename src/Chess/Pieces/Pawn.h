#pragma once
#include <Engine.h>
#include "ChessPiece.h"

class Pawn : public ChessPiece
{
private:
    bool m_hasMoved{false};

public:
    Pawn(const ChessBoard *board, const Engine::Vec2 &position, PieceColor color)
        : ChessPiece(board, position, ChessConstants::PAWN_SIZE, color) {}

    virtual void Initialize() override;

    virtual void Render() const override;

    virtual std::vector<Engine::Vec2> GetPossibleMoves() const override;

    virtual void OnMove(const Engine::Vec2 &newPosition) override;
};