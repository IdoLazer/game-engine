#pragma once

#include <Engine.h>
#include <Core/EntryPoint.h>

#include "InputManager.h"
#include "ChessBoard.h"

using namespace Engine;

class Chess : public Application
{
private:
    ChessBoard *m_board;
    InputManager *m_inputManager;

public:
    Chess() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() const override;
    void Shutdown() override;

    WindowConfig GetWindowConfig() const override;
};

namespace Engine
{
    Application *CreateApplication()
    {
        return new Chess();
    }
}