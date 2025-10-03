#pragma once
#include "../ChessBoard.h"
#include <vector>
#include <Engine.h>

using namespace Engine;

// Define white and black types
enum class ChessPieceColor
{
    White,
    Black
};

class ChessPiece : public GridTile
{
protected:
    ChessPieceColor m_ChessPieceColor;
    const ChessBoard *m_board;

public:
    ChessPiece(const ChessBoard *board, const Vec2 &position, const Vec2 &size, ChessPieceColor color)
        : GridTile(board, position, size), m_board(board), m_ChessPieceColor(color)
    {
        // Set the color of the chess piece
        m_color = (color == ChessPieceColor::White) ? Color::White : Color::Black;
    }

    ChessPiece() = default;

    virtual std::vector<Vec2> GetPossibleMoves() const = 0; // Pure virtual function for possible moves

    virtual void OnMove(const Vec2 &newPosition) = 0;

    void Select();
    void Deselect();
};