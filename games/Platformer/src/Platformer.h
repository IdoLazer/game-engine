#pragma once
#include <Engine.h>
#include <Core/EntryPoint.h>

// --- Forward Declarations ---
class PlatformerInputManager;
class Cursor;

// --- Platformer Application ---
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
    // --- Game Logic ---
    void GoToNextLevel(int row);
    void GoToPreviousLevel(int row);
    void ReloadCurrentLevel();

    enum class SpawnType { Entry, Return };

    // --- Fields ---
    int m_currentLevel = 0;
    bool m_hasSpawnOverride = false;
    SpawnType m_spawnType{SpawnType::Entry};
    int m_spawnRow{0};

    std::unique_ptr<PlatformerInputManager> m_inputManager;

    Engine::Subscription m_exitSub;
    Engine::Subscription m_nextLevelSub;
    Engine::Subscription m_previousLevelSub;
    Engine::Subscription m_reloadLevelSub;
    Engine::Subscription m_moveSub;
    Engine::Subscription m_jumpSub;
    Engine::Subscription m_jumpStopSub;
    Engine::Subscription m_cursorMoveSub;
    Engine::Subscription m_debugNextLevelSub;
    Engine::Subscription m_debugPreviousLevelSub;
    Engine::Subscription m_debugReloadLevelSub;

    Engine::Grid m_grid;
    Cursor *m_cursor = nullptr;
};
