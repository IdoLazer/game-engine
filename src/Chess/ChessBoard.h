#pragma once

#include <Engine.h>

using namespace Engine;

class ChessBoard : public Grid
{
public:
    ChessBoard(const Vec2 &position, float cellSize)
        : Grid(position, Color::White, cellSize, Vec2{8, 8})
    {
    }

    ~ChessBoard() = default;

    void Initialize() override;
    void Render() override;

private:
    std::vector<GridTile> m_Tiles;
};