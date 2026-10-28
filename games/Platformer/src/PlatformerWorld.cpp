#include "PlatformerWorld.h"
#include "PlatformerLayers.h"
#include "Levels/LevelExit.h"
#include "Levels/SpawnPoint.h"
#include <iostream>
#include <map>

BEGIN_TYPE_REGISTER(PlatformerWorld)
    REGISTER_PROPERTY(TileGrid, TileGrid, &PlatformerWorld::m_tileGrid)
    REGISTER_PROPERTY(Engine::Color, StaticTileColor, &PlatformerWorld::m_staticTileColor)
    REGISTER_PROPERTY(Engine::Color, DeathTileColor, &PlatformerWorld::m_deathTileColor)
END_TYPE_REGISTER()

void PlatformerWorld::Initialize()
{
    TileMap::Initialize();
    m_physics.SetTiles(*m_coordSystem, [this](const Engine::Vec2 &cell) { return IsSolid(cell); }, Layer::Terrain);
}

TileType PlatformerWorld::TileAt(const Engine::Vec2 &cell) const
{
    return m_tileGrid.At(static_cast<int>(cell.x), static_cast<int>(cell.y));
}

bool PlatformerWorld::IsSolid(const Engine::Vec2 &cell) const
{
    return TileAt(cell) == TileType::Solid;
}

bool PlatformerWorld::IsDeadly(const Engine::Vec2 &cell) const
{
    return TileAt(cell) == TileType::Death;
}

// --- Level Regions ---

void PlatformerWorld::PlaceRegions()
{
    std::map<char, Engine::Rect> unnamed = m_tileGrid.GetAnchors();

    for (LevelRegion *region : GetScene()->GetAllEntitiesOfType<LevelRegion>())
    {
        if (!region->Place(m_tileGrid))
            continue;

        unnamed.erase(region->GetAnchor());

        if (auto *spawnPoint = dynamic_cast<SpawnPoint *>(region))
        {
            unnamed.erase(spawnPoint->GetDefaultAnchor());
            m_spawnPoints.push_back(spawnPoint);
        }
        else if (auto *levelExit = dynamic_cast<LevelExit *>(region))
        {
            m_exits.push_back(levelExit);
        }
    }

    for (const auto &[glyph, bounds] : unnamed)
        std::cerr << "PlatformerWorld: '" << glyph << "' is drawn in the picture, but no entity names it" << std::endl;
}

Engine::Vec2 PlatformerWorld::FindSpawnCell(const std::string &spawnName, const std::optional<Engine::Vec2> &exitCell) const
{
    if (m_spawnPoints.empty())
    {
        std::cerr << "PlatformerWorld: the level has no SpawnPoint, placing the player at 1, 1" << std::endl;
        return Engine::Vec2(1.0f, 1.0f);
    }

    for (const SpawnPoint *spawnPoint : m_spawnPoints)
    {
        if (spawnPoint->GetName() == spawnName)
            return spawnPoint->GetSpawnCell(exitCell);
    }

    if (!spawnName.empty())
        std::cerr << "PlatformerWorld: no SpawnPoint named '" << spawnName << "', using the level's first" << std::endl;

    return m_spawnPoints.front()->GetSpawnCell(exitCell);
}

const LevelExit *PlatformerWorld::FindExitAt(const Engine::Vec2 &cell) const
{
    for (const LevelExit *levelExit : m_exits)
    {
        if (levelExit->Contains(cell))
            return levelExit;
    }
    return nullptr;
}

void PlatformerWorld::RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const
{
    switch (m_tileGrid.At(x, y))
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
