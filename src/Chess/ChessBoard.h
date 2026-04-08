#pragma once

#include <Engine.h>
#include <vector>
#include "ChessTile.h"
#include "ChessConstants.h"
using namespace Engine;

class ChessPiece; 
class ChessBoard : public Grid
{
public:
    ChessBoard(const Vec2 &position, float cellSize)
        : Grid(position, Color::White, cellSize, Vec2{8, 8})
    {
    }

    ~ChessBoard() = default;

    void Initialize() override;
    void Render() const override;
    void Update(float deltaTime) override;

    bool IsValidPosition(const Vec2 &gridPos) const;
    bool IsOccupied(const Vec2 &gridPos) const;
    void AddPiece(ChessPiece *piece);
    void OnMouseClick(const Vec2 &gridPos);
    ChessTile *GetTile(const Vec2 &gridPos) const;
    ChessPiece *GetPieceAt(const Vec2 &gridPos) const;
    bool IsGameOver() const { return m_gameOver; }
    ChessPieceColor GetCurrentPlayerColor() const { return m_currentPlayerColor; }

private:
    std::vector<ChessTile *> m_tiles;
    std::vector<ChessPiece *> m_whitePieces;
    std::vector<ChessPiece *> m_blackPieces;
    ChessPiece *m_selectedPiece{nullptr};
    ChessPieceColor m_currentPlayerColor{ChessPieceColor::White};
    bool m_gameOver{false};
};