#pragma once

#include <Engine.h>
#include <vector>
#include "ChessTile.h"
#include "ChessConstants.h"
#include "ChessTypes.h"

class ChessPiece; 
class ChessBoard : public Engine::Grid
{
public:
    ChessBoard(const Engine::Vec2 &position, float cellSize)
        : Engine::Grid(position, Engine::Color::White, cellSize, Engine::Vec2{ChessConstants::BOARD_SIZE, ChessConstants::BOARD_SIZE})
    {
    }

    ~ChessBoard() = default;

    void Initialize() override;
    void Render() const override;
    void Update(float deltaTime) override;

    bool IsValidPosition(const Engine::Vec2 &gridPos) const;
    bool IsOccupied(const Engine::Vec2 &gridPos) const;
    void AddPiece(ChessPiece *piece);
    void OnMouseClick(const Engine::Vec2 &gridPos);
    ChessTile *GetTile(const Engine::Vec2 &gridPos) const;
    ChessPiece *GetPieceAt(const Engine::Vec2 &gridPos) const;
    bool IsGameOver() const { return m_gameOver; }
    PieceColor GetCurrentPlayerColor() const { return m_currentPlayerColor; }

private:
    std::vector<ChessTile *> m_tiles;
    std::vector<ChessPiece *> m_whitePieces;
    std::vector<ChessPiece *> m_blackPieces;
    ChessPiece *m_selectedPiece{nullptr};
    PieceColor m_currentPlayerColor{PieceColor::White};
    bool m_gameOver{false};
};