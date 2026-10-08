#pragma once
#include <Engine.h>
#include <Core/EntryPoint.h>

// --- Forward Declarations ---
class PlatformerInputManager;
class PlatformerWorld;
class Player;
class Cursor;
class LevelSet;

// --- Platformer Application ---
class Platformer : public Engine::Application
{
public:
    // --- Constructors & Destructors ---
    Platformer();

    // --- Game Interface ---
    void Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    Engine::WindowConfig GetWindowConfig() const override;

private:
    // --- Game Logic ---
    void InstantiateCurrentLevel(const LevelSet &levelSet);
    void GoToNextLevel(int row);
    void GoToPreviousLevel(int row);
    void ReloadCurrentLevel();

    enum class SpawnType { Entry, Return };

    // --- Fields ---
    std::vector<std::string> m_levelPaths;
    int m_currentLevel = 0;
    bool m_hasSpawnOverride = false;
    SpawnType m_spawnType{SpawnType::Entry};
    int m_spawnRow{0};

    // Built in the constructor: Initialize and Shutdown run for every level load.
    std::unique_ptr<PlatformerInputManager> m_inputManager;
    std::vector<Engine::Subscription> m_inputSubscriptions;

    Engine::Grid m_grid;
    PlatformerWorld *m_world{nullptr};
    Player *m_player{nullptr};
    Cursor *m_cursor = nullptr;
};
