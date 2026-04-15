#include "ChessBoard.h"
// --- Type Registration ---

BEGIN_TYPE_REGISTER(ChessBoard)
END_TYPE_REGISTER()
                                                           
using namespace Engine;

// --- Lifecycle ---
void ChessBoard::Initialize()
{
    int total = ChessConstants::BOARD_SIZE * ChessConstants::BOARD_SIZE;
    m_highlighted.resize(total, false);
}

// --- Accessors ---

void ChessBoard::ToggleHighlight(const Vec2 &cell, bool highlighted)
{
    int x = static_cast<int>(cell.x);
    int y = static_cast<int>(cell.y);
    int idx = CellIndex(x, y);
    if (idx >= 0 && idx < static_cast<int>(m_highlighted.size()))
        m_highlighted[idx] = highlighted;
}

// --- Subclass Interface ---

void ChessBoard::RenderTile(int x, int y, const Vec2 &worldPos, const Vec2 &worldSize) const
{
    // Alternate light/dark colors
    Color tileColor = (x + y) % 2 == 0 ? ChessConstants::TILE_COLOR_LIGHT : ChessConstants::TILE_COLOR_DARK;

    // If this tile is highlighted, apply the highlight tint and draw an outline
    int idx = CellIndex(x, y);
    if (m_highlighted[idx])
        tileColor = tileColor + ChessConstants::TILE_HIGHLIGHT_TINT;

    Renderer2D::DrawTile(worldPos, worldSize, tileColor);

    if (m_highlighted[idx])
        Renderer2D::DrawRectOutline(worldPos, worldSize, ChessConstants::TILE_HIGHLIGHT_OUTLINE);
}

// --- Internal ---

int ChessBoard::CellIndex(int x, int y) const { return y * ChessConstants::BOARD_SIZE + x; }
