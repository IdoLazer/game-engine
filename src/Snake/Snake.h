#pragma once
#include <Engine.h>
#include <Core/EntryPoint.h>
#include <vector>
#include <functional>
#include "Player.h"
#include "Commands/InputManager.h"

class Snake : public Engine::Application
{
public:
    // --- Constructors & Destructors ---
    Snake() = default;

    // --- Game Interface ---
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() const override;
    void Shutdown() override;

    Engine::WindowConfig GetWindowConfig() const override;

private:
    // --- Initialization ---
    void InitializeWorld();
    void InitializePlayer();
    void InitializeFood();

    // --- Game Logic ---
    void ReadInput();
    bool CheckGameOver() const;
    bool CheckFoodCollision() const;
    bool CheckWallCollision() const;
    void PlaceFood();
    bool IsValidFoodPosition(const Engine::Vec2 &position) const;

    // --- Fields ---
    std::unique_ptr<InputManager> m_inputManager;
    Engine::Grid m_grid;
    Engine::Vec2 m_gridWorldSize;
    Engine::Color m_backgroundColor;
    Player *m_player{nullptr};
    Engine::GridTile *m_food{nullptr};
};