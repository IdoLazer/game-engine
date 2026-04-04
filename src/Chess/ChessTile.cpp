#include "ChessTile.h"

void ChessTile::Render() const
{
    GridTile::Render();

    if (m_isHighlighted)
    {
        Color highlightColor{Color(1, 1, 0, 0.8f)};

        // Draw the highlight as a frame around the tile
        Renderer2D::DrawRectOutline(m_worldPosition, m_worldSize, highlightColor);
    }
}