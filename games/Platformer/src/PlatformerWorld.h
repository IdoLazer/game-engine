#pragma once

#include <Engine.h>
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

// --- TileMap Interface ---
protected:
    void RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const override;

// --- Configuration ---
private:
    std::vector<std::vector<int>> m_tileGrid;
    Engine::Color m_color{};
    int m_rows{0};
    int m_cols{0};
};
