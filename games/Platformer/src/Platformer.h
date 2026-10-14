#pragma once
#include <Engine.h>
#include <Core/EntryPoint.h>

#include <optional>

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
    void GoToLevel(const std::string &level, const std::string &spawnName = "",
                   const std::optional<Engine::Vec2> &exitCell = std::nullopt);
    void GoToListedLevel(int step);
    void ReloadCurrentLevel();

    // --- Fields ---
    std::vector<std::string> m_levelPaths;
    std::string m_currentLevel;
    std::string m_spawnName;                // The SpawnPoint to appear at; empty for the level's first
    std::optional<Engine::Vec2> m_exitCell; // Where the player left the previous level

    std::unique_ptr<PlatformerInputManager> m_inputManager;
    std::vector<Engine::Subscription> m_inputSubscriptions;

    Engine::Grid m_grid;
    PlatformerWorld *m_world{nullptr};
    Player *m_player{nullptr};
    Cursor *m_cursor = nullptr;
};
