#pragma once

#include <Engine.h>

#include <string>

class PlatformerWorld;

// A level entity placed on an anchor: the cells of the level's picture drawn with one character.
class LevelRegion : public Engine::GridEntity
{
    DECLARE_TYPE(LevelRegion, GridEntity)

// --- Public Interface ---
public:
    // Covers the cells drawn with the region's anchor. False when `world` has no such anchor.
    virtual bool Place(PlatformerWorld &world);
    bool Contains(const Engine::Vec2 &cell) const;

    // '\0' unless the document gave exactly one character.
    char GetAnchor() const { return m_anchor.size() == 1 ? m_anchor[0] : '\0'; }

// --- Subclass Interface ---
protected:
    PlatformerWorld &GetWorld() const { return *m_world; }

// --- Configuration (data-driven via type registry) ---
private:
    std::string m_anchor;

// --- Private Fields ---
private:
    PlatformerWorld *m_world{nullptr};
};
