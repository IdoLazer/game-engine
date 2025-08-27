#include "Game.h"
#include <algorithm>
#include <iostream>

WindowConfig Game::GetWindowConfig() const
{
    WindowConfig config;

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

    // Initialize world
    float worldWidth = Renderer2D::GetCamera().GetWorldWidth();
    float worldHeight = Renderer2D::GetCamera().GetWorldHeight();
    int gridSizeX = static_cast<int>(worldWidth);
    int gridSizeY = static_cast<int>(worldHeight);
    m_GridSize = Vec2(gridSizeX, gridSizeY);
    m_GridCells = std::vector<std::vector<Vec2>>(gridSizeX, std::vector<Vec2>(gridSizeY, Vec2(0.0f, 0.0f)));
    for (int x = 0; x < gridSizeX; ++x)
    {
        for (int y = 0; y < gridSizeY; ++y)
        {
            m_GridCells[x][y] = Vec2(x - worldWidth * 0.5f + 0.5f, worldHeight * 0.5f - y - 0.5f);
        }
    }

    // Initialize player
    m_MoveDirection = Vec2(1.0f, 0.0f); // Moving right initially
    m_TailSegments = std::vector<Vec2>();

    // Initial food position at the center of the grid
    float foodX = static_cast<float>(gridSizeX / 2);
    float foodY = static_cast<float>(gridSizeY / 2);
    m_FoodCell = Vec2{foodX, foodY};
}

void Game::Update(float deltaTime)
{
    ReadInput();
    m_TimeSinceLastMove += deltaTime;
    if (m_TimeSinceLastMove > 1.0f / m_MoveSpeed)
    {
        m_TimeSinceLastMove = 0;
        MovePlayer();
    }
    if (CheckCollision())
    {
        PlaceFood();
        GrowPlayer();
    }
}

void Game::Render()
{
    Renderer2D::DrawTile(m_GridCells[m_PlayerCell.x][m_PlayerCell.y], m_PlayerSize, m_PlayerColor);
    Renderer2D::DrawTile(m_GridCells[m_FoodCell.x][m_FoodCell.y], m_FoodSize, m_FoodColor);
}

void Game::PlaceFood()
{
    // Randomly place food within grid
    int gridWidth = static_cast<int>(m_GridSize.x);
    int gridHeight = static_cast<int>(m_GridSize.y);
    float foodX = rand() % gridWidth;
    float foodY = rand() % gridHeight;

    m_FoodCell = Vec2{foodX, foodY};
}

void Game::ReadInput()
{
    // Exit game on Escape
    if (Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        std::cout << "Escape pressed - exiting game!" << std::endl;
        Close();
    }

    if (Keyboard::IsKeyPressed(GLFW_KEY_UP))
    {
        m_MoveDirection = Vec2(0.0f, -1.0f);
    }
    if (Keyboard::IsKeyPressed(GLFW_KEY_DOWN))
    {
        m_MoveDirection = Vec2(0.0f, 1.0f);
    }
    if (Keyboard::IsKeyPressed(GLFW_KEY_LEFT))
    {
        m_MoveDirection = Vec2(-1.0f, 0.0f);
    }
    if (Keyboard::IsKeyPressed(GLFW_KEY_RIGHT))
    {
        m_MoveDirection = Vec2(1.0f, 0.0f);
    }
}

void Game::MovePlayer()
{
    m_PlayerCell += m_MoveDirection;
    m_PlayerCell = Vec2{std::clamp(m_PlayerCell.x, 0.0f, m_GridSize.x - 1), std::clamp(m_PlayerCell.y, 0.0f, m_GridSize.y - 1)};
}

bool Game::CheckCollision() const
{
    // Simple AABB collision detection
    return m_PlayerCell == m_FoodCell;
}

void Game::GrowPlayer()
{
    m_TailSegments.push_back(m_PlayerCell);
}

void Game::Shutdown()
{
    std::cout << "Game shutting down." << std::endl;
    m_TailSegments.clear();
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