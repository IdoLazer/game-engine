#pragma once

#include <Engine.h>
#include "TileType.h"
#include "Levels/TileGrid.h"
#include "Physics/Physics.h"

#include <optional>
#include <set>
#include <string>
#include <vector>

// --- Forward Declarations ---
class ILevelNavigator;
class Session;
class SpawnPoint;

class PlatformerWorld : public Engine::TileMap
{
    DECLARE_TYPE(PlatformerWorld, TileMap)

// --- Constructors & Destructors ---
public:
    PlatformerWorld() = default;
    ~PlatformerWorld() override = default;

// --- Lifecycle ---
public:
    void Initialize() override;
    void Update(float deltaTime) override;

// --- Collision Interface ---
public:
    bool IsSolid(const Engine::Vec2 &cell) const;

// --- Level Regions ---
public:
    // Places every LevelRegion in the scene on its anchor. Call once the level's entities exist.
    void PlaceRegions();

    // The box of the cells drawn with `glyph`, marked as named. Empty when none are.
    std::optional<Engine::Rect> ClaimAnchor(char glyph);

    // The cell the player appears in: at the SpawnPoint called `spawnName`, or at the level's
    // first when the name is empty. `exitCell` is where the player left the previous level.
    Engine::Vec2 FindSpawnCell(const std::string &spawnName, const std::optional<Engine::Vec2> &exitCell) const;

    // Tells every switchable region called `name` to switch. Reports a name no region has.
    void Switch(const std::string &name, bool on);

// --- Accessors ---
public:
    Engine::Vec2 GetCellCount() const { return Engine::Vec2(m_tileGrid.GetColumnCount(), m_tileGrid.GetRowCount()); }
    Physics &GetPhysics() { return m_physics; }
    void SetNavigator(ILevelNavigator *navigator) { m_navigator = navigator; }
    ILevelNavigator &GetNavigator() const { return *m_navigator; }
    void SetSession(Session *session) { m_session = session; }
    Session &GetSession() const { return *m_session; }

// --- TileMap Interface ---
protected:
    void RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const override;

// --- Internal ---
private:
    TileType TileAt(const Engine::Vec2 &cell) const;
    TileCollision CollisionAt(const Engine::Vec2 &cell) const;
    void OnColliderCrushed(const Collider &collider);
    void OnColliderEnteredTile(const Collider &collider, const Engine::Vec2 &cell);

// --- Configuration ---
private:
    TileGrid m_tileGrid;
    Engine::Color m_staticTileColor{};
    Engine::Color m_deathTileColor{};

// --- Private Fields ---
private:
    Physics m_physics;
    // After m_physics, so they unsubscribe while its events still exist.
    Engine::Subscription m_crushedSubscription;
    Engine::Subscription m_tileEnteredSubscription;
    std::set<char> m_claimedAnchors;
    std::vector<SpawnPoint *> m_spawnPoints;
    ILevelNavigator *m_navigator{nullptr};
    Session *m_session{nullptr};
};
