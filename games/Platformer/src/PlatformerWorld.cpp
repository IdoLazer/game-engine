#include "PlatformerWorld.h"

BEGIN_TYPE_REGISTER(PlatformerWorld)
    REGISTER_PROPERTY(std::vector<std::vector<int>>, TileGrid, &PlatformerWorld::m_tileGrid)
    REGISTER_PROPERTY(Engine::Color, StaticTileColor, &PlatformerWorld::m_staticTileColor)
    REGISTER_PROPERTY(Engine::Color, DeathTileColor, &PlatformerWorld::m_deathTileColor)
END_TYPE_REGISTER()

void PlatformerWorld::SetTileGrid(const std::vector<std::vector<int>> &grid)
{
    m_tileGrid = grid;
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

bool PlatformerWorld::IsNextLevel(const Engine::Vec2 &cell) const
{
    int cx = static_cast<int>(cell.x);
    int cy = static_cast<int>(cell.y);
    if (cy < 0 || cy >= m_rows || cx < 0 || cx >= m_cols)
        return false;
    return GetTileAt(cx, cy) == TileType::NextLevel;
}

bool PlatformerWorld::IsDeadly(const Engine::Vec2 &cell) const
{
    int cx = static_cast<int>(cell.x);
    int cy = static_cast<int>(cell.y);
    if (cy < 0 || cy >= m_rows || cx < 0 || cx >= m_cols)
        return false;
    return GetTileAt(cx, cy) == TileType::Death;
}

bool PlatformerWorld::IsPreviousLevel(const Engine::Vec2 &cell) const
{
    int cx = static_cast<int>(cell.x);
    int cy = static_cast<int>(cell.y);
    if (cy < 0 || cy >= m_rows || cx < 0 || cx >= m_cols)
        return false;
    return GetTileAt(cx, cy) == TileType::PreviousLevel;
}

Engine::Vec2 PlatformerWorld::FindEntrySpawn(int row) const
{
    for (int x = 0; x < m_cols; ++x)
    {
        if (GetTileAt(x, row) == TileType::EntrySpawn)
            return Engine::Vec2(static_cast<float>(x), static_cast<float>(row));
    }
    return FindDefaultSpawn();
}

Engine::Vec2 PlatformerWorld::FindReturnSpawn(int row) const
{
    for (int x = 0; x < m_cols; ++x)
    {
        if (GetTileAt(x, row) == TileType::ReturnSpawn)
            return Engine::Vec2(static_cast<float>(x), static_cast<float>(row));
    }
    return FindDefaultSpawn();
}

Engine::Vec2 PlatformerWorld::FindDefaultSpawn() const
{
    for (int y = 0; y < m_rows; ++y)
        for (int x = 0; x < m_cols; ++x)
            if (GetTileAt(x, y) == TileType::DefaultSpawn)
                return Engine::Vec2(static_cast<float>(x), static_cast<float>(y));

    for (int y = 0; y < m_rows; ++y)
        for (int x = 0; x < m_cols; ++x)
            if (GetTileAt(x, y) == TileType::EntrySpawn)
                return Engine::Vec2(static_cast<float>(x), static_cast<float>(y));

    return Engine::Vec2(1.0f, 1.0f);
}

void PlatformerWorld::RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const
{
    switch (GetTileAt(x, y))
    {
    case TileType::Solid:
        Engine::Renderer2D::DrawTile(worldPos, worldSize, m_staticTileColor);
        break;
    case TileType::Death:
        Engine::Renderer2D::DrawTile(worldPos, worldSize, m_deathTileColor);
        break;
    default:
        break;
    }
}

TileType PlatformerWorld::GetTileAt(int x, int y) const
{
    return static_cast<TileType>(m_tileGrid[y][x]);
}
