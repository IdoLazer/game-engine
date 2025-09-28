#include "ChessBoard.h"

void ChessBoard::Initialize()
{
    Grid::Initialize();

    Color black = Color(139.0f / 255.0f, 0, 0);                             // Dark Red
    Color white = Color(198.0f / 255.0f, 218.0f / 255.0f, 167.0f / 255.0f); // Beige

    m_Tiles.reserve(8 * 8);
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            Color tileColor = (x + y) % 2 == 0 ? white : black;
            m_Tiles.emplace_back(this, Vec2{x, y}, Vec2{1, 1}, tileColor);
        }
    }
}

void ChessBoard::Render()
{
    for (const auto &tile : m_Tiles)
    {
        tile.Render();
    }
}