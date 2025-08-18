#pragma once
#include "Engine/Core/Application.h"

class Game : public Engine::Application
{

public:
    Game() = default;
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Shutdown() override;

    Engine::WindowConfig GetWindowConfig() const override;

private:
    Engine::Vec2 m_PlayerPosition;
    Engine::Vec2 m_PlayerSize;
};