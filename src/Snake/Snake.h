#pragma once
#include <Engine.h>
#include <Core/EntryPoint.h>
#include <vector>
#include <functional>
#include "Player.h"
#include "Commands/MovementInputHandler.h"

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
    bool CheckGameOver() const;
    bool CheckFoodCollision() const;
    bool CheckWallCollision() const;
    void PlaceFood();
    bool IsValidFoodPosition(const Engine::Vec2 &position) const;

    // --- Fields ---
    std::unique_ptr<MovementInputHandler> m_inputHandler;
    Engine::Subscription m_escapeSub;
    Engine::Grid m_grid;
    Engine::Vec2 m_gridWorldSize;
    Engine::Color m_backgroundColor;
    Player *m_player{nullptr};
    Engine::GridTile *m_food{nullptr};
};