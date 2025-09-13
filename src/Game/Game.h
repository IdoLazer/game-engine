#pragma once
#include <Engine.h>
#include <Core/EntryPoint.h>
#include <vector>
#include <functional>
#include "Timer.h"
#include "Grid.h"

using namespace Engine;

// Game Constants
namespace GameConstants
{
    // Player Settings
    const Vec2 PLAYER_SIZE{1.0f, 1.0f};
    const Color PLAYER_COLOR{1.0f, 0.0f, 0.0f, 1.0f}; // Red
    static constexpr int INITIAL_TAIL_LENGTH = 1;
    static constexpr float MOVE_SPEED = 6.0f; // Grid cells per second

    // Food Settings
    const Vec2 FOOD_SIZE{0.5f, 0.5f};
    const Color FOOD_COLOR{0.0f, 1.0f, 0.0f, 1.0f}; // Green

    // World Settings
    const Vec2 GRID_CELL_COUNT{13.0f, 10.0f};
    const Color BACKGROUND_COLOR{0.0f, 0.0f, 1.0f, 1.0f}; // Blue

    // Movement
    const Vec2 DIRECTION_UP{0.0f, -1.0f};
    const Vec2 DIRECTION_DOWN{0.0f, 1.0f};
    const Vec2 DIRECTION_LEFT{-1.0f, 0.0f};
    const Vec2 DIRECTION_RIGHT{1.0f, 0.0f};
}

class Game : public Engine::Application
{
private:
    void ReadInput();
    void MovePlayer();
    bool CheckGameOver() const;
    bool CheckFoodCollision() const;
    bool CheckWallCollision() const;
    bool CheckSelfCollision() const;
    void GrowPlayer();
    void PlaceFood();

    // Timer callbacks
    void OnMoveTimer();

    // Initialization helpers
    void InitializeWorld();
    void InitializePlayer();
    void InitializeFood();

    // Validation helpers
    bool IsValidFoodPosition(const Vec2 &position) const;

public:
    Game() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Shutdown() override;

    WindowConfig GetWindowConfig() const override;

private:
    // World
    Grid m_Grid; // Our grid coordinate system

    // Player
    Vec2 m_PlayerCell; // Start at top-left corner
    Vec2 m_MoveDirection;
    std::vector<Vec2> m_TailSegments;
    Timer m_MoveTimer{1.0f / GameConstants::MOVE_SPEED, [this]()
                      { OnMoveTimer(); }, true};
    bool m_UpdateMoveThisFrame{false};

    // Food
    Vec2 m_FoodCell;
};