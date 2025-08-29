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
    m_MarginX = (worldWidth - gridSizeX) * 0.5f;
    m_GridSize = Vec2(gridSizeX, gridSizeY);

    // print the grid size, the margin and the world width
    std::cout << "Grid Size: " << m_GridSize.x << "x" << m_GridSize.y << std::endl;
    std::cout << "Margin: " << m_MarginX << std::endl;
    std::cout << "World Width: " << worldWidth << std::endl;

    m_GridCells = std::vector<std::vector<Vec2>>(gridSizeX, std::vector<Vec2>(gridSizeY, Vec2(0.0f, 0.0f)));
    for (int x = 0; x < gridSizeX; ++x)
    {
        for (int y = 0; y < gridSizeY; ++y)
        {
            m_GridCells[x][y] = Vec2(x - worldWidth * 0.5f + 0.5f + m_MarginX, worldHeight * 0.5f - y - 0.5f);
        }
    }

    // Initialize player
    m_MoveDirection = Vec2(1.0f, 0.0f); // Moving right initially
    // Initialize with 1 tail segment
    m_TailSegments = std::vector<Vec2>(m_StartTailLength, Vec2(0.0f, 0.0f));
    for (int i = 0; i < m_StartTailLength; ++i)
    {
        m_TailSegments[i] = Vec2(i, 0.0f);
    }
    m_PlayerCell = Vec2(m_StartTailLength, 0.0f);

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
        if (CheckGameOver())
        {
            std::cout << "Game Over!" << std::endl;
            Close();
        }
    }
    if (CheckCollision())
    {
        PlaceFood();
        GrowPlayer();
    }
}

void Game::Render()
{
    // Draw Player
    Renderer2D::DrawTile(m_GridCells[m_PlayerCell.x][m_PlayerCell.y], m_PlayerSize, m_PlayerColor);
    Renderer2D::DrawTile(m_GridCells[m_FoodCell.x][m_FoodCell.y], m_FoodSize, m_FoodColor);
    for (const Vec2 &segment : m_TailSegments)
    {
        Renderer2D::DrawTile(m_GridCells[segment.x][segment.y], m_PlayerSize, m_PlayerColor);
    }

    // Draw world boundaries
    Renderer2D::DrawTile(0.5f * Vec2{-m_GridSize.x - m_MarginX, 0.0f}, Vec2{m_MarginX, m_GridSize.y}, Color{0.0f, 0.0f, 1.0f, 1.0f});
    Renderer2D::DrawTile(0.5f * Vec2{m_GridSize.x + m_MarginX, 0.0f}, Vec2{m_MarginX, m_GridSize.y}, Color{0.0f, 0.0f, 1.0f, 1.0f});
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

    if (m_UpdateMoveThisFrame)
        return;

    if (Keyboard::IsKeyPressed(GLFW_KEY_UP) && m_MoveDirection.y != 1)
    {
        m_UpdateMoveThisFrame = true;
        m_MoveDirection = Vec2(0.0f, -1.0f);
    }
    if (Keyboard::IsKeyPressed(GLFW_KEY_DOWN) && m_MoveDirection.y != -1)
    {
        m_UpdateMoveThisFrame = true;
        m_MoveDirection = Vec2(0.0f, 1.0f);
    }
    if (Keyboard::IsKeyPressed(GLFW_KEY_LEFT) && m_MoveDirection.x != 1)
    {
        m_UpdateMoveThisFrame = true;
        m_MoveDirection = Vec2(-1.0f, 0.0f);
    }
    if (Keyboard::IsKeyPressed(GLFW_KEY_RIGHT) && m_MoveDirection.x != -1)
    {
        m_UpdateMoveThisFrame = true;
        m_MoveDirection = Vec2(1.0f, 0.0f);
    }
}

void Game::MovePlayer()
{
    // Move the tail segments by shifting them forward and placing the previous head position at the front
    if (!m_TailSegments.empty())
    {
        std::rotate(m_TailSegments.rbegin(), m_TailSegments.rbegin() + 1, m_TailSegments.rend());
        m_TailSegments.front() = m_PlayerCell;
    }
    m_PlayerCell += m_MoveDirection;
    m_PlayerCell = Vec2{std::clamp(m_PlayerCell.x, 0.0f, m_GridSize.x - 1), std::clamp(m_PlayerCell.y, 0.0f, m_GridSize.y - 1)};
    m_UpdateMoveThisFrame = false;
}

bool Game::CheckCollision() const
{
    // Simple AABB collision detection
    return m_PlayerCell == m_FoodCell;
}

bool Game::CheckGameOver() const
{
    // Check if the player has collided with the walls
    if (m_PlayerCell.x < 0 || m_PlayerCell.x >= m_GridSize.x ||
        m_PlayerCell.y < 0 || m_PlayerCell.y >= m_GridSize.y)
    {
        return true;
    }

    // Check if the player has collided with its own tail
    for (const Vec2 &segment : m_TailSegments)
    {
        if (m_PlayerCell == segment)
        {
            return true;
        }
    }

    return false;
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