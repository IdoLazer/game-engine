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
};