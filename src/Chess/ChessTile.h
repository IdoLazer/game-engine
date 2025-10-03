#pragma once

#include <Engine.h>

using namespace Engine;

class ChessTile : public GridTile
{
public:
    using GridTile::GridTile;

    virtual void Render() override;

    void ToggleHighlight(bool highlighted) { m_isHighlighted = highlighted; }

private:
    bool m_isHighlighted = false;
};