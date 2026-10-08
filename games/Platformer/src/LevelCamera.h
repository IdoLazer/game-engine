#pragma once

#include <Engine.h>

// Keeps the engine camera on a target, without showing past the level's edges.
class LevelCamera : public Engine::Entity
{
    DECLARE_TYPE(LevelCamera, Entity)

// --- Lifecycle ---
public:
    void Update(float deltaTime) override;
    void Destroy() override;

// --- Public Interface ---
public:
    void Follow(const Engine::Entity *target, const Engine::Rect &levelBounds);
    float GetVisibleRows() const { return m_visibleRows; }

// --- Configuration (data-driven via type registry) ---
private:
    float m_visibleRows{20.0f}; // Rows of the level that fit the screen's height

// --- Private Fields ---
private:
    const Engine::Entity *m_target{nullptr};
    Engine::Rect m_levelBounds{}; // World units
};
