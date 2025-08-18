#include "Game.h"
#include "Engine/Input/Keyboard.h"
#include "Engine/Rendering/Renderer2D.h"
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

    m_PlayerPosition = Engine::Vec2(50, 50);
    m_PlayerSize = Engine::Vec2(5, 5);
}

void Game::Update()
{
    // Exit game on Escape
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        std::cout << "Escape pressed - exiting game!" << std::endl;
        Close();
    }

    // Move player position based on arrow keys
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_UP))
    {
        m_PlayerPosition.y += 1;
    }
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_DOWN))
    {
        m_PlayerPosition.y -= 1;
    }
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_LEFT))
    {
        m_PlayerPosition.x -= 1;
    }
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_RIGHT))
    {
        m_PlayerPosition.x += 1;
    }

    // Update player size based on input (for testing)
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_W))
    {
        m_PlayerSize.y += 1;
    }
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_S))
    {
        m_PlayerSize.y -= 1;
    }
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_A))
    {
        m_PlayerSize.x -= 1;
    }
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_D))
    {
        m_PlayerSize.x += 1;
    }
}

void Game::Render()
{
    // Test our tile rendering with world coordinates (0-100 scale)

    // Red tile at player position with player size
    Engine::Renderer2D::DrawTile(m_PlayerPosition, m_PlayerSize, Engine::Color(1.0f, 0.0f, 0.0f, 1.0f));
}

void Game::Shutdown()
{
    std::cout << "Game shutting down." << std::endl;
}