#pragma once

#include <Engine.h>
#include "TileType.h"
#include <vector>

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
    bool IsNextLevel(const Engine::Vec2 &cell) const;
    bool IsPreviousLevel(const Engine::Vec2 &cell) const;
    bool IsDeadly(const Engine::Vec2 &cell) const;

    // Swept collision against solid tiles - the shared basis for all entity-vs-world collision.
    // `direction` must be normalized; RaycastSolid's `maxDistance` scales it.
    bool OverlapsSolid(const Engine::Rect &box) const;
    Engine::SweepHit SweepSolid(const Engine::Rect &movingRect, const Engine::Vec2 &delta) const;
    Engine::SweepHit RaycastSolid(const Engine::Vec2 &origin, const Engine::Vec2 &direction, float maxDistance) const;

    // Static contact test: true if `box` is touching solid, even flush with no overlap - unlike
    // SweepSolid, which excludes that case (see Sweep.cpp). `direction` must be normalized.
    bool TouchesSolid(const Engine::Rect &box, const Engine::Vec2 &direction) const;

    Engine::Vec2 FindEntrySpawn(int row) const;
    Engine::Vec2 FindReturnSpawn(int row) const;
    Engine::Vec2 FindDefaultSpawn() const;
    void SetTileGrid(const std::vector<std::vector<int>> &grid);

// --- TileMap Interface ---
protected:
    void RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const override;

// --- Internal ---
private:
    TileType GetTileAt(int x, int y) const;

// --- Configuration ---
private:
    std::vector<std::vector<int>> m_tileGrid;
    Engine::Color m_staticTileColor{};
    Engine::Color m_deathTileColor{};
    int m_rows{0};
    int m_cols{0};
};
