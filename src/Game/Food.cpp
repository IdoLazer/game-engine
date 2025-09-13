#include "Food.h"

void Food::Initialize(Grid *grid, const Vec2 &startPos, const Vec2 &size, const Color &color)
{
    m_grid = grid;
    m_position = startPos;
    m_size = size;
    m_color = color;
}

void Food::Render() const
{
    Renderer2D::DrawTile(m_grid->GridToWorld(m_position), m_grid->GetCellSize() * m_size, m_color);
}
