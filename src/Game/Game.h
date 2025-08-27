#pragma once
#include <Engine.h> // Elegant system header syntax!
#include <vector>

using namespace Engine;

class Game : public Engine::Application
{
private:
    void ReadInput();
    void MovePlayer();
    bool CheckCollision() const;
    void GrowPlayer();
    void PlaceFood();

public:
    Game() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Shutdown() override;

    WindowConfig GetWindowConfig() const override;

private:
    // World
    Vec2 m_GridSize;
    Vec2 m_GridCellSize{1.0f, 1.0f};
    std::vector<std::vector<Vec2>> m_GridCells;

    // Player
    Vec2 m_PlayerCell{0.0f, 0.0f}; // Start at top-left corner
    Vec2 m_PlayerSize{1.0f, 1.0f}; // 1x1 world unit tile
    Color m_PlayerColor{1.0f, 0.0f, 0.0f, 1.0f};
    Vec2 m_MoveDirection;
    float m_MoveSpeed{5.0f}; // 5 grid cells per second
    std::vector<Vec2> m_TailSegments;
    float m_TimeSinceLastMove{0.0f};

    // Food
    Vec2 m_FoodCell;             // Start food at center (0,0)
    Vec2 m_FoodSize{0.5f, 0.5f}; // 0.5x0.5 world unit tile
    Color m_FoodColor{0.0f, 1.0f, 0.0f, 1.0f};
};