#pragma once

#include <Engine.h>
#include "ChessConstants.h"

class ChessTile : public Engine::GridTile
{
public:
    using Engine::GridTile::GridTile;

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