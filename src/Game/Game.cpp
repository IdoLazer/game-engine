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
    // Draw grid boundaries for visual reference
    Vec2 gridWorldSize = m_Grid.GetSize();
    Vec2 gridCenter = m_Grid.GetPosition();
    Renderer2D::DrawTile(gridCenter, gridWorldSize, GameConstants::BACKGROUND_COLOR);

    // Draw Player - use Grid to convert grid coordinates to world positions
    Renderer2D::DrawTile(m_Grid.GridToWorld(m_PlayerCell), m_Grid.GetCellSize() * GameConstants::PLAYER_SIZE, GameConstants::PLAYER_COLOR);
    for (const Vec2 &segment : m_TailSegments)
    {
        Renderer2D::DrawTile(m_Grid.GridToWorld(segment), m_Grid.GetCellSize() * GameConstants::PLAYER_SIZE, GameConstants::PLAYER_COLOR);
    }
    // Draw Food
    Renderer2D::DrawTile(m_Grid.GridToWorld(m_FoodCell), m_Grid.GetCellSize() * GameConstants::FOOD_SIZE, GameConstants::FOOD_COLOR);
}

void Game::PlaceFood()
{
    // Randomly place food within grid, ensuring it doesn't overlap with the player or tail
    const int maxAttempts = 100; // Safety limit to prevent infinite loops
    int attempts = 0;

    do
    {
        Vec2 cellCount = m_Grid.GetCellCount();
        int gridWidth = static_cast<int>(cellCount.x);
        int gridHeight = static_cast<int>(cellCount.y);
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
    // Check if the player has collided with the walls using Grid's boundary checking
    return !m_Grid.IsInBounds(m_PlayerCell);
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
    float worldWidth = Renderer2D::GetCamera().GetWorldWidth();
    float worldHeight = Renderer2D::GetCamera().GetWorldHeight();

    // Calculate cell size to fit grid in viewport with margins
    float availableWidth = worldWidth * 0.8f;
    float availableHeight = worldHeight * 0.8f;

    float cellSizeX = availableWidth / GameConstants::GRID_CELL_COUNT.x;
    float cellSizeY = availableHeight / GameConstants::GRID_CELL_COUNT.y;
    float cellSize = std::min(cellSizeX, cellSizeY); // Fit both dimensions

    Vec2 gridPosition = Vec2(0.0f, 0.0f);
    m_Grid.Initialize(gridPosition, cellSize, GameConstants::GRID_CELL_COUNT);
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
    Vec2 cellCount = m_Grid.GetCellCount();
    int foodX = static_cast<int>(cellCount.x / 2);
    int foodY = static_cast<int>(cellCount.y / 2);
    m_FoodCell = Vec2{foodX, foodY};
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