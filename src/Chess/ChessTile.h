#pragma once

#include <Engine.h>

using namespace Engine;

class ChessTile : public GridTile
{
public:
    using GridTile::GridTile;

    virtual void Render() const override;

    void ToggleHighlight(bool highlighted)
    {
        Color highlightColor{Color(0.1f, 0.1f, 0, 1)};
        if (highlighted && !m_isHighlighted)
        {
            m_color = m_color + highlightColor;
        }
        else if (!highlighted && m_isHighlighted)
        {
            m_color = m_color - highlightColor;
        }
        m_isHighlighted = highlighted;
    }

private:
    bool m_isHighlighted = false;
};