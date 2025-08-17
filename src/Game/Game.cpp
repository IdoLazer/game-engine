#include "Game.h"
#include "Engine/Input/Keyboard.h"
#include <iostream>

Engine::WindowConfig Game::GetWindowConfig() const
{
    Engine::WindowConfig config;

    config.title = "Game";
    config.width = 800;
    config.height = 600;
    config.fullscreen = false;
    config.resizable = true;
    return config;
}

void Game::Initialize()
{
    std::cout << "Game initialized." << std::endl;
}

void Game::Update()
{
    // Exit game on Escape
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        std::cout << "Escape pressed - exiting game!" << std::endl;
        Close();
    }
}

void Game::Render()
{
    // Render your game here
    // The window clearing and buffer swapping is handled by the Application class
}

void Game::Shutdown()
{
    std::cout << "Game shutting down." << std::endl;
}