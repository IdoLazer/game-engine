#pragma once
#include "../ChessBoard.h"
#include <vector>
#include <Engine.h>
#include "../ChessConstants.h"
#include "../ChessTypes.h"

using namespace Engine;

class ChessPiece : public GridTile
{
protected:
    PieceColor m_pieceColor;
    const ChessBoard *m_board;

public:
    ChessPiece(const ChessBoard *board, const Vec2 &position, const Vec2 &size, PieceColor color)
        : GridTile(board, position, size), m_board(board), m_pieceColor(color)
    {
        // Set the color of the chess piece
        m_color = (color == PieceColor::White) ? ChessConstants::PIECE_COLOR_WHITE : ChessConstants::PIECE_COLOR_BLACK;
    }

    ChessPiece() = default;

    virtual std::vector<Vec2> GetPossibleMoves() const = 0; // Pure virtual function for possible moves

    virtual void OnMove(const Vec2 &newPosition)
    {
        SetGridPosition(newPosition);
    }

    void Select();
    void Deselect();

    PieceColor GetPieceColor() const { return m_pieceColor; }

protected:
    // Movement helpers — subclasses call these with their direction arrays
    std::vector<Vec2> GetSlidingMoves(const std::vector<Vec2> &directions) const;
    std::vector<Vec2> GetSteppingMoves(const std::vector<Vec2> &directions) const;
};