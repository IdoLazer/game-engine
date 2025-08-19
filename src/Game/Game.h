#pragma once
#include "Engine/Core/Application.h"

class Game : public Engine::Application
{

public:
    Game() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Shutdown() override;

    Engine::WindowConfig GetWindowConfig() const override;

private:
    Engine::Vec2 m_PlayerPosition{50, 50};
    Engine::Vec2 m_PlayerSize{5, 5};
    Engine::Vec2 m_MoveDirection{1.0f, 0.0f};
    float m_MoveSpeed{50.0f};
};