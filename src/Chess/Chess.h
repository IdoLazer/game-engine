#pragma once

#include <Engine.h>
#include <Core/EntryPoint.h>

#include "InputManager.h"
#include "ChessBoard.h"

class Chess : public Engine::Application
{
private:
    ChessBoard *m_board{nullptr};
    InputManager m_inputManager;

public:
    Chess() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() const override;
    void Shutdown() override {};

    Engine::WindowConfig GetWindowConfig() const override;
};

namespace Engine
{
    Application *CreateApplication()
    {
        return new Chess();
    }
}