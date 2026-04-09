#pragma once
#include "../ChessBoard.h"
#include <vector>
#include <Engine.h>
#include "../ChessConstants.h"
#include "../ChessTypes.h"

class ChessPiece : public Engine::GridTile
{
protected:
    PieceColor m_pieceColor;
    const ChessBoard *m_board;

public:
    ChessPiece(const ChessBoard *board, const Engine::Vec2 &position, const Engine::Vec2 &size, PieceColor color)
        : Engine::GridTile(board, position, size), m_board(board), m_pieceColor(color)
    {
        // Set the color of the chess piece
        m_color = (color == PieceColor::White) ? ChessConstants::PIECE_COLOR_WHITE : ChessConstants::PIECE_COLOR_BLACK;
    }

    ChessPiece() = default;

    virtual std::vector<Engine::Vec2> GetPossibleMoves() const = 0; // Pure virtual function for possible moves

    virtual void OnMove(const Engine::Vec2 &newPosition)
    {
        SetGridPosition(newPosition);
    }

    void Select();
    void Deselect();

    PieceColor GetPieceColor() const { return m_pieceColor; }

protected:
    // Movement helpers — subclasses call these with their direction arrays
    std::vector<Engine::Vec2> GetSlidingMoves(const std::vector<Engine::Vec2> &directions) const;
    std::vector<Engine::Vec2> GetSteppingMoves(const std::vector<Engine::Vec2> &directions) const;
};