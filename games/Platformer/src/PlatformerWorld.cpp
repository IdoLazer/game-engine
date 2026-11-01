#include "PlatformerWorld.h"
#include "ILevelNavigator.h"
#include "PlatformerLayers.h"
#include "Elements/ISwitchable.h"
#include "Levels/SpawnPoint.h"
#include <iostream>

BEGIN_TYPE_REGISTER(PlatformerWorld)
    REGISTER_PROPERTY(TileGrid, TileGrid, &PlatformerWorld::m_tileGrid)
    REGISTER_PROPERTY(Engine::Color, StaticTileColor, &PlatformerWorld::m_staticTileColor)
    REGISTER_PROPERTY(Engine::Color, DeathTileColor, &PlatformerWorld::m_deathTileColor)
END_TYPE_REGISTER()

void PlatformerWorld::Initialize()
{
    TileMap::Initialize();
    m_physics.SetTiles(*m_coordSystem, [this](const Engine::Vec2 &cell) { return CollisionAt(cell); }, Layer::Terrain, Layer::Player);
    m_crushedSubscription = m_physics.OnCrushed().Subscribe(this, &PlatformerWorld::OnColliderCrushed);
    m_tileEnteredSubscription = m_physics.OnTileEntered().Subscribe(this, &PlatformerWorld::OnColliderEnteredTile);
}

void PlatformerWorld::Update(float deltaTime)
{
    m_physics.Update();
}

void PlatformerWorld::OnColliderCrushed(const Collider &collider)
{
    if (collider.GetLayer() & Layer::Player)
        GetNavigator().ReloadCurrentLevel();
}

void PlatformerWorld::OnColliderEnteredTile(const Collider &collider, const Engine::Vec2 &cell)
{
    GetNavigator().ReloadCurrentLevel();
}

TileType PlatformerWorld::TileAt(const Engine::Vec2 &cell) const
{
    return m_tileGrid.At(static_cast<int>(cell.x), static_cast<int>(cell.y));
}

TileCollision PlatformerWorld::CollisionAt(const Engine::Vec2 &cell) const
{
    switch (TileAt(cell))
    {
    case TileType::Solid:
        return TileCollision::Solid;
    case TileType::Death:
        return TileCollision::Trigger;
    default:
        return TileCollision::None;
    }
}

bool PlatformerWorld::IsSolid(const Engine::Vec2 &cell) const
{
    return TileAt(cell) == TileType::Solid;
}

// --- Level Regions ---

void PlatformerWorld::PlaceRegions()
{
    for (LevelRegion *region : GetScene()->GetAllEntitiesOfType<LevelRegion>())
    {
        if (!region->Place(*this))
            continue;

        if (auto *spawnPoint = dynamic_cast<SpawnPoint *>(region))
            m_spawnPoints.push_back(spawnPoint);
    }

    for (const auto &[glyph, bounds] : m_tileGrid.GetAnchors())
    {
        if (!m_claimedAnchors.contains(glyph))
            std::cerr << "PlatformerWorld: '" << glyph << "' is drawn in the picture, but no entity names it" << std::endl;
    }
}

std::optional<Engine::Rect> PlatformerWorld::ClaimAnchor(char glyph)
{
    std::optional<Engine::Rect> bounds = m_tileGrid.FindAnchor(glyph);
    if (bounds)
        m_claimedAnchors.insert(glyph);

    return bounds;
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

void PlatformerWorld::Switch(const std::string &name, bool on)
{
    bool found = false;

    for (LevelRegion *region : GetScene()->GetAllEntitiesOfType<LevelRegion>())
    {
        // Regions without a Name have an empty one, which no switch may match.
        if (name.empty() || region->GetName() != name)
            continue;

        found = true;
        if (auto *switchable = dynamic_cast<ISwitchable *>(region))
            switchable->OnSwitched(on);
    }

    if (!found)
        std::cerr << "PlatformerWorld: no region named '" << name << "' to switch" << std::endl;
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
