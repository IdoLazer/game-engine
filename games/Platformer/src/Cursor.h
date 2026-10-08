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
    void Update(float deltaTime) override;
    void Render() const override;
};
