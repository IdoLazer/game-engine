#pragma once

#include "GridEntity.h"
#include "../Rendering/Renderer2D.h"

namespace Engine
{
    class GridTile : public GridEntity
    {
    public:
        using GridEntity::GridEntity; // Inherit constructors

        void Render() const
        {
            Renderer2D::DrawTile(m_worldPosition, m_grid->GetCellSize() * m_gridSize, m_color);
        }
    };
}