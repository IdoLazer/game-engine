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
    int m_rows{0};
    int m_cols{0};
};
