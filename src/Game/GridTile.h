#pragma once

#include <Engine.h>
#include "Grid.h"

using namespace Engine;

class GridTile
{
private:
    Grid *m_grid;
    Vec2 m_position;
    Vec2 m_size;
    Color m_color;

public:
    GridTile(Grid *grid, const Vec2 &position, const Vec2 &size, const Color &color)
        : m_grid(grid), m_position(position), m_size(size), m_color(color)
    {
    }

    void Render() const
    {
        Renderer2D::DrawTile(m_grid->GridToWorld(m_position), m_grid->GetCellSize() * m_size, m_color);
    }

    // Getters
    Vec2 GetPosition() const { return m_position; }
    Vec2 GetSize() const { return m_size; }
    Color GetColor() const { return m_color; }

    // Setters
    void SetPosition(const Vec2 &pos) { m_position = pos; }
    void SetSize(const Vec2 &size) { m_size = size; }
    void SetColor(const Color &color) { m_color = color; }
};
