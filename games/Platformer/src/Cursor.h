#pragma once

#include <Engine.h>

class Cursor : public Engine::Entity
{
    DECLARE_TYPE(Cursor, Entity)

// --- Fields ---
private:
    float m_radius{0.0f};

// --- Lifecycle ---
public:
    void Render() const override;

// --- Setters ---
public:
    void SetPosition(const Engine::Vec2 &position) { m_worldPosition = position; }
};
