#include "PlatformerWorld.h"

BEGIN_TYPE_REGISTER(PlatformerWorld)
    REGISTER_PROPERTY(std::vector<std::vector<int>>, TileGrid, &PlatformerWorld::m_tileGrid)
    REGISTER_PROPERTY(Engine::Color, Color, &PlatformerWorld::m_color)
END_TYPE_REGISTER()

void PlatformerWorld::Initialize()
{
    m_rows = static_cast<int>(m_tileGrid.size());
    m_cols = m_rows > 0 ? static_cast<int>(m_tileGrid[0].size()) : 0;
}

bool PlatformerWorld::IsSolid(const Engine::Vec2 &cell) const
{
    int cx = static_cast<int>(cell.x);
    int cy = static_cast<int>(cell.y);
    if (cy < 0 || cy >= m_rows || cx < 0 || cx >= m_cols)
        return false;
    return m_tileGrid[cy][cx] == 1;
}

void PlatformerWorld::RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const
{
    if (m_tileGrid[y][x] == 1)
        Engine::Renderer2D::DrawTile(worldPos, worldSize, m_color);
}
