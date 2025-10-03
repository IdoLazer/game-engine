#pragma once

#include "GridEntity.h"
#include "../Rendering/Renderer2D.h"

namespace Engine
{
    class GridTile : public GridEntity
    {
    public:
        using GridEntity::GridEntity; // Inherit constructors

        virtual void Render() override
        {
            Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);
        }
    };
}