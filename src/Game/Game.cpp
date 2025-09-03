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

    InitializeWorld();
    InitializePlayer();
    InitializeFood();
}

void Game::Update(float deltaTime)
{
    ReadInput();
    m_MoveTimer.Update(deltaTime);
}

void Game::OnMoveTimer()
{
    MovePlayer();

    if (CheckGameOver())
    {
        std::cout << "Game Over!" << std::endl;
        Close();
    }
    else if (CheckFoodCollision())
    {
        PlaceFood();
        GrowPlayer();
    }
}

void Game::Render()
{
    // Draw Player
    Renderer2D::DrawTile(m_GridCells[m_PlayerCell.x][m_PlayerCell.y], GameConstants::PLAYER_SIZE, GameConstants::PLAYER_COLOR);
    Renderer2D::DrawTile(m_GridCells[m_FoodCell.x][m_FoodCell.y], GameConstants::FOOD_SIZE, GameConstants::FOOD_COLOR);
    for (const Vec2 &segment : m_TailSegments)
    {
        Renderer2D::DrawTile(m_GridCells[segment.x][segment.y], GameConstants::PLAYER_SIZE, GameConstants::PLAYER_COLOR);
    }

    // Draw world boundaries
    Renderer2D::DrawTile(0.5f * Vec2{-m_GridSize.x - m_MarginX, 0.0f}, Vec2{m_MarginX, m_GridSize.y}, GameConstants::BOUNDARY_COLOR);
    Renderer2D::DrawTile(0.5f * Vec2{m_GridSize.x + m_MarginX, 0.0f}, Vec2{m_MarginX, m_GridSize.y}, GameConstants::BOUNDARY_COLOR);
}

void Game::PlaceFood()
{
    // Randomly place food within grid, ensuring it doesn't overlap with the player or tail
    const int maxAttempts = 100; // Safety limit to prevent infinite loops
    int attempts = 0;

    do
    {
        int gridWidth = static_cast<int>(m_GridSize.x);
        int gridHeight = static_cast<int>(m_GridSize.y);
        float foodX = static_cast<float>(rand() % gridWidth);
        float foodY = static_cast<float>(rand() % gridHeight);
        m_FoodCell = Vec2{foodX, foodY};

        attempts++;
    } while (!IsValidFoodPosition(m_FoodCell) && attempts < maxAttempts);

    // If we couldn't find a valid position, just place it anyway (game is probably about to end)
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

    // Handle direction input - prevent immediate reversal into opposite direction
    if (Keyboard::IsKeyPressed(GLFW_KEY_UP) && m_MoveDirection != GameConstants::DIRECTION_DOWN)
    {
        m_UpdateMoveThisFrame = true;
        m_MoveDirection = GameConstants::DIRECTION_UP;
    }
    else if (Keyboard::IsKeyPressed(GLFW_KEY_DOWN) && m_MoveDirection != GameConstants::DIRECTION_UP)
    {
        m_UpdateMoveThisFrame = true;
        m_MoveDirection = GameConstants::DIRECTION_DOWN;
    }
    else if (Keyboard::IsKeyPressed(GLFW_KEY_LEFT) && m_MoveDirection != GameConstants::DIRECTION_RIGHT)
    {
        m_UpdateMoveThisFrame = true;
        m_MoveDirection = GameConstants::DIRECTION_LEFT;
    }
    else if (Keyboard::IsKeyPressed(GLFW_KEY_RIGHT) && m_MoveDirection != GameConstants::DIRECTION_LEFT)
    {
        m_UpdateMoveThisFrame = true;
        m_MoveDirection = GameConstants::DIRECTION_RIGHT;
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
    m_UpdateMoveThisFrame = false;
}

bool Game::CheckGameOver() const
{
    return CheckWallCollision() || CheckSelfCollision();
}

bool Game::CheckFoodCollision() const
{
    return m_PlayerCell == m_FoodCell;
}

bool Game::CheckWallCollision() const
{
    // Check if the player has collided with the walls
    return m_PlayerCell.x < 0 || m_PlayerCell.x >= m_GridSize.x ||
           m_PlayerCell.y < 0 || m_PlayerCell.y >= m_GridSize.y;
}

bool Game::CheckSelfCollision() const
{
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

void Game::InitializeWorld()
{
    // Set up grid based on camera world size
    float worldWidth = Renderer2D::GetCamera().GetWorldWidth();
    float worldHeight = Renderer2D::GetCamera().GetWorldHeight();
    int gridSizeX = static_cast<int>(worldWidth);
    int gridSizeY = static_cast<int>(worldHeight);
    m_MarginX = (worldWidth - gridSizeX) * 0.5f;
    m_GridSize = Vec2(gridSizeX, gridSizeY);

    // Debug output for world setup
    std::cout << "Grid Size: " << m_GridSize.x << "x" << m_GridSize.y << std::endl;
    std::cout << "Margin: " << m_MarginX << std::endl;
    std::cout << "World Width: " << worldWidth << std::endl;

    // Create grid cells with world positions
    m_GridCells = std::vector<std::vector<Vec2>>(gridSizeX, std::vector<Vec2>(gridSizeY, Vec2(0.0f, 0.0f)));
    for (int x = 0; x < gridSizeX; ++x)
    {
        for (int y = 0; y < gridSizeY; ++y)
        {
            m_GridCells[x][y] = GridToWorldPosition(x, y, worldWidth, worldHeight);
        }
    }
}

void Game::InitializePlayer()
{
    // Set initial movement direction
    m_MoveDirection = GameConstants::DIRECTION_RIGHT;

    // Create initial tail segments
    m_TailSegments = std::vector<Vec2>(GameConstants::INITIAL_TAIL_LENGTH, Vec2(0.0f, 0.0f));
    for (int i = 0; i < GameConstants::INITIAL_TAIL_LENGTH; ++i)
    {
        m_TailSegments[i] = Vec2(i, 0);
    }

    // Position player head
    m_PlayerCell = Vec2(GameConstants::INITIAL_TAIL_LENGTH, 0);
}

void Game::InitializeFood()
{
    // Place initial food at center of grid
    int foodX = static_cast<int>(m_GridSize.x / 2);
    int foodY = static_cast<int>(m_GridSize.y / 2);
    m_FoodCell = Vec2{foodX, foodY};
}

Vec2 Game::GridToWorldPosition(int gridX, int gridY, float worldWidth, float worldHeight) const
{
    // Convert grid coordinates to world coordinates
    // Grid origin (0,0) is at top-left, world origin (0,0) is at center
    float worldX = gridX - worldWidth * 0.5f + 0.5f + m_MarginX;
    float worldY = worldHeight * 0.5f - gridY - 0.5f;
    return Vec2(worldX, worldY);
}

bool Game::IsValidFoodPosition(const Vec2 &position) const
{
    // Check if position doesn't overlap with player or tail segments
    if (position == m_PlayerCell)
        return false;

    return std::find(m_TailSegments.begin(), m_TailSegments.end(), position) == m_TailSegments.end();
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