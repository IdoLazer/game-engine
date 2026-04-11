#pragma once
#include <vector>
#include <Engine.h>
#include "../ChessConstants.h"
#include "../ChessTypes.h"


class ChessPiece : public Engine::GridEntity
{
protected:
    PieceColor m_pieceColor;

public:
    ChessPiece(Engine::Grid *grid, const Engine::Vec2 &position, const Engine::Vec2 &size, PieceColor color)
        : Engine::GridEntity(grid, position, size), m_pieceColor(color)
    {
        m_color = (color == PieceColor::White) ? ChessConstants::PIECE_COLOR_WHITE : ChessConstants::PIECE_COLOR_BLACK;
    }

    ChessPiece() = default;

    virtual std::vector<Engine::Vec2> GetPossibleMoves() const = 0;

    PieceColor GetPieceColor() const { return m_pieceColor; }

protected:
    // Movement helpers — subclasses call these with their direction arrays
    std::vector<Engine::Vec2> GetSlidingMoves(const std::vector<Engine::Vec2> &directions) const;
    std::vector<Engine::Vec2> GetSteppingMoves(const std::vector<Engine::Vec2> &directions) const;

    // Spatial query helpers using Grid
    bool IsCellEmpty(const Engine::Vec2 &pos) const;
    bool IsCellValidAndEmpty(const Engine::Vec2 &pos) const;
    ChessPiece *GetPieceAt(const Engine::Vec2 &pos) const;
};