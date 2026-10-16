#pragma once

#include <Engine.h>

class Cursor : public Engine::Entity
{
    DECLARE_TYPE(Cursor, Entity)

// --- Fields ---
private:
    float m_radius{0.0f};
    Engine::Vec2 m_screenPosition{};

// --- Lifecycle ---
public:
    void Update(float deltaTime) override;
    void Render() const override;

// --- Setters ---
public:
    void SetScreenPosition(const Engine::Vec2 &screenPosition) { m_screenPosition = screenPosition; }
};
