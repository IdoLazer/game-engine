#pragma once

#include <Engine.h>
#include <Core/EntryPoint.h>

using namespace Engine;

class Chess : public Application
{
public:
    Chess() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
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