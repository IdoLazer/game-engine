#pragma once

#include "ChessPiece.h"

class King : public ChessPiece
{
private:
    bool m_hasMoved{false};

public:
    King(Engine::Grid *grid, const Engine::Vec2 &position, PieceColor color)
        : ChessPiece(grid, position, ChessConstants::KING_SIZE, color) {}

    virtual void Initialize() override;
    virtual void Render() const override;
    virtual std::vector<Engine::Vec2> GetPossibleMoves() const override;
    virtual void SetGridPosition(const Engine::Vec2 &newPosition) override;

    bool HasMoved() const { return m_hasMoved; }
};