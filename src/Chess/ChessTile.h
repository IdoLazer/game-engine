#pragma once

#include <Engine.h>
#include "ChessConstants.h"

using namespace Engine;

class ChessTile : public GridTile
{
public:
    using GridTile::GridTile;

    virtual void Render() const override;

    void ToggleHighlight(bool highlighted)
    {
        if (highlighted && !m_isHighlighted)
        {
            m_color = m_color + ChessConstants::TILE_HIGHLIGHT_TINT;
        }
        else if (!highlighted && m_isHighlighted)
        {
            m_color = m_color - ChessConstants::TILE_HIGHLIGHT_TINT;
        }
        m_isHighlighted = highlighted;
    }

private:
    bool m_isHighlighted = false;
};