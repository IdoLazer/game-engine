#pragma once
#include <Engine.h>
#include <Core/EntryPoint.h>

class Platformer : public Engine::Application
{
public:
    // --- Constructors & Destructors ---
    Platformer() = default;

    // --- Game Interface ---
    void Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    Engine::WindowConfig GetWindowConfig() const override;

private:
    // --- Initialization ---

    // --- Game Logic ---

    // --- Fields ---
    Engine::Subscription m_exitSub;
    Engine::Grid m_grid;
};