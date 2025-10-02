#pragma once

#include <Engine.h>
#include <vector>
using namespace Engine;

class ChessBoard : public Grid
{
private:
    std::vector<class ChessPiece *> m_pieces;

public:
    ChessBoard(const Vec2 &position, float cellSize)
        : Grid(position, Color::White, cellSize, Vec2{8, 8})
    {
    }

    ~ChessBoard() = default;

    void Initialize() override;
    void Render() override;

    bool IsValidPosition(const Vec2 &gridPos) const;
    void AddPiece(class ChessPiece *piece);

private:
    std::vector<GridTile> m_tiles;
};