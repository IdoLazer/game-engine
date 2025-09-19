#pragma once
#include <Engine.h>
#include <Core/EntryPoint.h>
#include <vector>
#include <functional>
#include <memory>
#include "Grid.h"
#include "Player.h"

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
    void Render() override;
    void Shutdown() override;

    WindowConfig GetWindowConfig() const override;

private:
    // World
    std::unique_ptr<Grid> m_Grid; // Our grid coordinate system

    // Player
    std::unique_ptr<Player> m_Player;

    // Food
    std::unique_ptr<GridTile> m_food;
};