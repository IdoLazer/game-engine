#pragma once

#include <Engine.h>
#include <vector>
#include "ChessConstants.h"

class ChessBoard : public Engine::TileMap
{
public:
    ChessBoard(const Engine::GridCoordinateSystem *coordSystem)
        : Engine::TileMap(coordSystem)
    {
        int total = ChessConstants::BOARD_SIZE * ChessConstants::BOARD_SIZE;
        m_highlighted.resize(total, false);
    }

    void ToggleHighlight(const Engine::Vec2 &cell, bool highlighted);

protected:
    void RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const override;

private:
    std::vector<bool> m_highlighted;

    int CellIndex(int x, int y) const { return y * ChessConstants::BOARD_SIZE + x; }
};