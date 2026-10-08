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

    // Which of a level's spawn points the player appears at.
    struct Spawn
    {
        std::string name;                  // Empty picks the level's first SpawnPoint
        std::optional<Engine::Vec2> carry; // Where in the exit the player left it, 0 to 1 per axis
    };

    // A LevelExit with its anchor resolved against the level's tile grid.
    struct ExitZone
    {
        Engine::Rect bounds;
        std::string targetLevel;
        std::string targetSpawn;
    };

    void GoToLevel(const std::string &level, Spawn spawn);
    void GoToListedLevel(int step);
    void ReloadCurrentLevel();

    // --- Fields ---
    std::vector<std::string> m_levelPaths;
    std::string m_currentLevel;
    Spawn m_spawn;
    std::vector<ExitZone> m_exits;

    // Built in the constructor: Initialize and Shutdown run for every level load.
    std::unique_ptr<PlatformerInputManager> m_inputManager;
    std::vector<Engine::Subscription> m_inputSubscriptions;

    Engine::Grid m_grid;
    PlatformerWorld *m_world{nullptr};
    Player *m_player{nullptr};
    Cursor *m_cursor = nullptr;
};
