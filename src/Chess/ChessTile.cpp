#include "ChessTile.h"

void ChessTile::Render() const
{
    GridTile::Render();

    if (m_isHighlighted)
    {
        Renderer2D::DrawRectOutline(m_worldPosition, m_worldSize, ChessConstants::TILE_HIGHLIGHT_OUTLINE);
    }
}