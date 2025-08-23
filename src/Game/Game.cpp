#include "Game.h"
#include <algorithm>
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

void Game::Update(float deltaTime)
{
    // Exit game on Escape
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        std::cout << "Escape pressed - exiting game!" << std::endl;
        Close();
    }

    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_UP))
    {
        m_MoveDirection = Engine::Vec2(0.0f, 1.0f);
    }
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_DOWN))
    {
        m_MoveDirection = Engine::Vec2(0.0f, -1.0f);
    }
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_LEFT))
    {
        m_MoveDirection = Engine::Vec2(-1.0f, 0.0f);
    }
    if (Engine::Keyboard::IsKeyPressed(GLFW_KEY_RIGHT))
    {
        m_MoveDirection = Engine::Vec2(1.0f, 0.0f);
    }

    // Clamp player position within camera bounds (centered coordinates)
    // Player position is now the CENTER of the tile
    float worldWidth = Engine::Renderer2D::GetCamera().GetWorldWidth();
    float worldHeight = Engine::Renderer2D::GetCamera().GetWorldHeight();

    float halfWidth = worldWidth * 0.5f;
    float halfHeight = worldHeight * 0.5f;
    float tileHalfWidth = m_PlayerSize.x * 0.5f;
    float tileHalfHeight = m_PlayerSize.y * 0.5f;

    m_PlayerPosition.x = std::clamp(m_PlayerPosition.x + m_MoveDirection.x * m_MoveSpeed * deltaTime,
                                    -halfWidth + tileHalfWidth, halfWidth - tileHalfWidth);
    m_PlayerPosition.y = std::clamp(m_PlayerPosition.y + m_MoveDirection.y * m_MoveSpeed * deltaTime,
                                    -halfHeight + tileHalfHeight, halfHeight - tileHalfHeight);
}

void Game::Render()
{
    // Test our new camera-based coordinate system with centered positioning

    // Red square centered at player position - should always be square regardless of window size!
    Engine::Renderer2D::DrawTile(m_PlayerPosition, m_PlayerSize, Engine::Color(1.0f, 0.0f, 0.0f, 1.0f));
}

void Game::Shutdown()
{
    std::cout << "Game shutting down." << std::endl;
}

// Engine Entry Point Factory Function
// This is called by the engine's main() to create our game instance
namespace Engine
{
    Application *CreateApplication()
    {
        return new Game();
    }
}