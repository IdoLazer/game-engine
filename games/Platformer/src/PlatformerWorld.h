#pragma once

#include <Engine.h>
#include "TileType.h"
#include "Levels/TileGrid.h"

#include <optional>
#include <string>
#include <vector>

// --- Forward Declarations ---
class LevelExit;
class SpawnPoint;

class PlatformerWorld : public Engine::TileMap
{
    DECLARE_TYPE(PlatformerWorld, TileMap)

// --- Constructors & Destructors ---
public:
    PlatformerWorld() = default;
    ~PlatformerWorld() override = default;

// --- Collision Interface ---
public:
    bool IsSolid(const Engine::Vec2 &cell) const;
    bool IsDeadly(const Engine::Vec2 &cell) const;

    // Swept collision against solid tiles - the shared basis for all entity-vs-world collision.
    // `direction` must be normalized; RaycastSolid's `maxDistance` scales it.
    bool OverlapsSolid(const Engine::Rect &box) const;
    Engine::SweepHit SweepSolid(const Engine::Rect &movingRect, const Engine::Vec2 &delta) const;
    Engine::SweepHit RaycastSolid(const Engine::Vec2 &origin, const Engine::Vec2 &direction, float maxDistance) const;

    // Static contact test: true if `box` is touching solid, even flush with no overlap - unlike
    // SweepSolid, which excludes that case (see Sweep.cpp). `direction` must be normalized.
    bool TouchesSolid(const Engine::Rect &box, const Engine::Vec2 &direction) const;

// --- Level Regions ---
public:
    // Places every LevelRegion in the scene on its anchor. Call once the level's entities exist.
    void PlaceRegions();

    // The cell the player appears in: at the SpawnPoint called `spawnName`, or at the level's
    // first when the name is empty. `exitCell` is where the player left the previous level.
    Engine::Vec2 FindSpawnCell(const std::string &spawnName, const std::optional<Engine::Vec2> &exitCell) const;

    // The exit `cell` is in, or null.
    const LevelExit *FindExitAt(const Engine::Vec2 &cell) const;

// --- Accessors ---
public:
    Engine::Vec2 GetCellCount() const { return Engine::Vec2(m_tileGrid.GetColumnCount(), m_tileGrid.GetRowCount()); }

// --- TileMap Interface ---
protected:
    void RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const override;

// --- Internal ---
private:
    TileType TileAt(const Engine::Vec2 &cell) const;

// --- Configuration ---
private:
    TileGrid m_tileGrid;
    Engine::Color m_staticTileColor{};
    Engine::Color m_deathTileColor{};

// --- Private Fields ---
private:
    std::vector<SpawnPoint *> m_spawnPoints;
    std::vector<LevelExit *> m_exits;
};
