#pragma once

#include <Engine.h>

#include <string>

class TileGrid;

// A level entity placed on an anchor: the cells of the level's picture drawn with one character.
class LevelRegion : public Engine::GridEntity
{
    DECLARE_TYPE(LevelRegion, GridEntity)

// --- Public Interface ---
public:
    // Covers the cells drawn with the region's anchor. False when `tiles` has no such anchor.
    bool Place(const TileGrid &tiles);
    bool Contains(const Engine::Vec2 &cell) const;

    // '\0' unless the document gave exactly one character.
    char GetAnchor() const { return m_anchor.size() == 1 ? m_anchor[0] : '\0'; }

// --- Configuration (data-driven via type registry) ---
private:
    std::string m_anchor;
};
