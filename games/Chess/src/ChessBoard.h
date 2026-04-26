#pragma once

#include <Engine.h>
#include <vector>
#include "ChessConstants.h"

class ChessBoard : public Engine::TileMap
{
    DECLARE_TYPE(ChessBoard, TileMap)

// --- Fields ---
private:
    std::vector<bool> m_highlighted;

// --- Constructors & Destructors ---
public:
    ChessBoard() = default;

// --- Lifecycle ---
public:
    void Initialize() override;

// --- Accessors ---
public:
    void ToggleHighlight(const Engine::Vec2 &cell, bool highlighted);

// --- Subclass Interface ---
protected:
    void RenderTile(int x, int y, const Engine::Vec2 &worldPos, const Engine::Vec2 &worldSize) const override;

// --- Internal ---
private:
    int CellIndex(int x, int y) const;
};