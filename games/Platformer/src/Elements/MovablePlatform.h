#pragma once

#include "../Levels/LevelRegion.h"
#include "../Physics/Collider.h"
#include "../PlatformerLayers.h"

#include <string>

// A solid that moves toward its end while switched on, and back to its start while off.
class MovablePlatform : public LevelRegion
{
    DECLARE_TYPE(MovablePlatform, LevelRegion)

// --- Lifecycle ---
public:
    void Update(float deltaTime) override;
    void Render() const override;

// --- Public Interface ---
public:
    // Starts where the Anchor is drawn and ends at the center of the box drawn with the EndAnchor.
    bool Place(PlatformerWorld &world) override;

// --- Internal ---
private:
    // '\0' unless the document gave exactly one character.
    char GetEndAnchor() const { return m_endAnchor.size() == 1 ? m_endAnchor[0] : '\0'; }

// --- Configuration (data-driven via type registry) ---
private:
    std::string m_endAnchor;
    float m_moveSpeed{0.0f}; // Cells per second
    bool m_switchedOn{false};

// --- Private Fields ---
private:
    Engine::Vec2 m_start{};
    Engine::Vec2 m_end{};
    Collider m_collider{*this, Layer::Terrain, 0};
};
