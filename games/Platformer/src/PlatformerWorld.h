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
class LevelExit;
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

// --- Collision Interface ---
public:
    bool IsSolid(const Engine::Vec2 &cell) const;
    bool IsDeadly(const Engine::Vec2 &cell) const;

// --- Level Regions ---
public:
    // Places every LevelRegion in the scene on its anchor. Call once the level's entities exist.
    void PlaceRegions();

    // The box of the cells drawn with `glyph`, marked as named. Empty when none are.
    std::optional<Engine::Rect> ClaimAnchor(char glyph);

    // The cell the player appears in: at the SpawnPoint called `spawnName`, or at the level's
    // first when the name is empty. `exitCell` is where the player left the previous level.
    Engine::Vec2 FindSpawnCell(const std::string &spawnName, const std::optional<Engine::Vec2> &exitCell) const;

    // The exit `cell` is in, or null.
    const LevelExit *FindExitAt(const Engine::Vec2 &cell) const;

// --- Accessors ---
public:
    Engine::Vec2 GetCellCount() const { return Engine::Vec2(m_tileGrid.GetColumnCount(), m_tileGrid.GetRowCount()); }
    Physics &GetPhysics() { return m_physics; }
    void SetNavigator(ILevelNavigator *navigator) { m_navigator = navigator; }
    ILevelNavigator &GetNavigator() const { return *m_navigator; }

// --- TileMap Interface ---
protected:
    void RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const override;

// --- Internal ---
private:
    TileType TileAt(const Engine::Vec2 &cell) const;
    void OnColliderCrushed(const Collider &collider);

// --- Configuration ---
private:
    TileGrid m_tileGrid;
    Engine::Color m_staticTileColor{};
    Engine::Color m_deathTileColor{};

// --- Private Fields ---
private:
    Physics m_physics;
    Engine::Subscription m_crushedSubscription; // After m_physics, so it unsubscribes while the event still exists.
    std::set<char> m_claimedAnchors;
    std::vector<SpawnPoint *> m_spawnPoints;
    std::vector<LevelExit *> m_exits;
    ILevelNavigator *m_navigator{nullptr};
};
