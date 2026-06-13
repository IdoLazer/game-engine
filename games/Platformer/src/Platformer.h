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

    enum class SpawnType { Entry, Return };

    // --- Fields ---
    int m_currentLevel = 0;
    bool m_hasSpawnOverride = false;
    SpawnType m_spawnType{SpawnType::Entry};
    int m_spawnRow{0};
    Engine::Subscription m_exitSub;
    Engine::Subscription m_nextLevelSub;
    Engine::Subscription m_previousLevelSub;
    Engine::Grid m_grid;
};