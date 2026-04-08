#pragma once
#include <Engine.h>
#include <Core/EntryPoint.h>
#include <vector>
#include <functional>
#include "Player.h"
#include "Commands/InputManager.h"

using namespace Engine;

class Game : public Engine::Application
{
private:
    void ReadInput();
    bool CheckGameOver() const;
    bool CheckFoodCollision() const;
    bool CheckWallCollision() const;
    void PlaceFood();

    // Initialization helpers
    void InitializeWorld();
    void InitializeFood();

    // Validation helpers
    bool IsValidFoodPosition(const Vec2 &position) const;

public:
    Game() = default;
    void Initialize() override;
    void InitializePlayer();
    void Update(float deltaTime) override;
    void Render() const override;
    void Shutdown() override;

    WindowConfig GetWindowConfig() const override;

private:
    // Input Management
    std::unique_ptr<InputManager> m_inputManager;

    // World
    Grid *m_Grid{nullptr};

    // Player
    Player *m_Player{nullptr};

    // Food
    GridTile *m_food{nullptr};
};