#pragma once
#include <Engine.h> // Elegant system header syntax!

class Game : public Engine::Application
{
private:
    void ReadInput();
    void MovePlayer(float deltaTime);
    bool CheckCollision() const;
    void GrowPlayer();
    void PlaceFood();

public:
    Game() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Shutdown() override;

    Engine::WindowConfig GetWindowConfig() const override;

private:
    Engine::Vec2 m_PlayerPosition{0.0f, 0.0f}; // Start at center (0,0)
    Engine::Vec2 m_PlayerSize{1.0f, 1.0f};     // 1x1 world unit tile
    Engine::Vec2 m_MoveDirection{1.0f, 0.0f};
    float m_MoveSpeed{5.0f};                 // 5 world units per second
    Engine::Vec2 m_FoodPosition{0.0f, 0.0f}; // Start food at center (0,0)
    Engine::Vec2 m_FoodSize{0.5f, 0.5f};     // 0.5x0.5 world unit tile
};