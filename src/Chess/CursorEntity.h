#pragma once

#include <Engine.h>

class CursorEntity : public Engine::Entity
{
    DECLARE_TYPE(CursorEntity, Entity)

// --- Fields ---
private:
    float m_radius{0.1f};

// --- Lifecycle ---
public:
    void Update(float deltaTime) override;
    void Render() const override;

// --- Accessors ---
public:
    void SetRadius(float radius);
};
