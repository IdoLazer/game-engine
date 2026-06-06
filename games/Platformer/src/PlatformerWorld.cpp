#include "PlatformerWorld.h"

BEGIN_TYPE_REGISTER(PlatformerWorld)
    REGISTER_PROPERTY(std::vector<std::vector<int>>, TileGrid, &PlatformerWorld::m_tileGrid)
    REGISTER_PROPERTY(Engine::Color, StaticTileColor, &PlatformerWorld::m_staticTileColor)
    REGISTER_PROPERTY(Engine::Color, LevelEndColor, &PlatformerWorld::m_levelEndColor)
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
    return GetTileAt(cx, cy) == TileType::Solid;
}

bool PlatformerWorld::IsLevelEnd(const Engine::Vec2 &cell) const
{
    int cx = static_cast<int>(cell.x);
    int cy = static_cast<int>(cell.y);
    if (cy < 0 || cy >= m_rows || cx < 0 || cx >= m_cols)
        return false;
    return GetTileAt(cx, cy) == TileType::LevelEnd;
}

Engine::Vec2 PlatformerWorld::FindSpawnPosition() const
{
    for (int y = 0; y < m_rows; ++y)
    {
        for (int x = 0; x < m_cols; ++x)
        {
            if (GetTileAt(x, y) == TileType::Spawn)
                return Engine::Vec2(static_cast<float>(x), static_cast<float>(y));
        }
    }
    return Engine::Vec2(1.0f, 1.0f); // Fallback if no spawn tile found
}

void PlatformerWorld::RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const
{
    switch (GetTileAt(x, y))
    {
    case TileType::Solid:
        Engine::Renderer2D::DrawTile(worldPos, worldSize, m_staticTileColor);
        break;
    case TileType::LevelEnd:
        Engine::Renderer2D::DrawTile(worldPos, worldSize, m_levelEndColor);
        break;
    default:
        break;
    }
}

TileType PlatformerWorld::GetTileAt(int x, int y) const
{
    return static_cast<TileType>(m_tileGrid[y][x]);
}
