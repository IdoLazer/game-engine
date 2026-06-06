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

// --- Lifecycle ---
public:
    void Initialize() override;

// --- Collision Interface ---
public:
    bool IsSolid(const Engine::Vec2 &cell) const;
    bool IsLevelEnd(const Engine::Vec2 &cell) const;
    Engine::Vec2 FindSpawnPosition() const;

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
    Engine::Color m_levelEndColor{};
    int m_rows{0};
    int m_cols{0};
};
