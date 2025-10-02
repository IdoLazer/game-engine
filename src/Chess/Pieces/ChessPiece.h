#pragma once
#include "../ChessBoard.h"
#include <vector>
#include <Engine.h>

using namespace Engine;
class ChessPiece : public GridTile
{
protected:
    const ChessBoard *m_board;

public:
    ChessPiece(const ChessBoard *board, const Vec2 &position, const Vec2 &size, const Color &color)
        : GridTile(board, position, size, color), m_board(board) {}

    ChessPiece() = default;

    virtual std::vector<Vec2> GetPossibleMoves() const = 0; // Pure virtual function for possible moves

    virtual void OnMove(const Vec2 &newPosition) = 0;
};