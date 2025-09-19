#include "Game.h"
#include <algorithm>
#include <iostream>
#include "GameConstants.h"

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
    m_Player->Update(deltaTime);
    ReadInput();
    if (CheckGameOver())
    {
        std::cout << "Game Over!" << std::endl;
        Close();
    }
    else if (CheckFoodCollision())
    {
        PlaceFood();
        m_Player->Grow();
    }
}

void Game::Render()
{
    // Draw grid
    m_Grid->Render();

    // Draw Player - use Grid to convert grid coordinates to world positions
    m_Player->Render();

    // Draw Food
    m_food->Render();
}

void Game::PlaceFood()
{
    // Randomly place food within grid, ensuring it doesn't overlap with the player or tail
    const int maxAttempts = 100; // Safety limit to prevent infinite loops
    int attempts = 0;
    Vec2 foodPosition;
    do
    {
        Vec2 cellCount = m_Grid->GetCellCount();
        int gridWidth = static_cast<int>(cellCount.x);
        int gridHeight = static_cast<int>(cellCount.y);
        float foodX = static_cast<float>(rand() % gridWidth);
        float foodY = static_cast<float>(rand() % gridHeight);
        foodPosition = Vec2{foodX, foodY};

        attempts++;
    } while (!IsValidFoodPosition(foodPosition) && attempts < maxAttempts);
    m_food->SetGridPosition(foodPosition);

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

    // Handle direction input - prevent immediate reversal into opposite direction
    if (Keyboard::IsKeyPressed(GLFW_KEY_UP) && m_Player->GetDirection() != GameConstants::DIRECTION_DOWN)
    {
        m_Player->SetDirection(GameConstants::DIRECTION_UP);
    }
    else if (Keyboard::IsKeyPressed(GLFW_KEY_DOWN) && m_Player->GetDirection() != GameConstants::DIRECTION_UP)
    {
        m_Player->SetDirection(GameConstants::DIRECTION_DOWN);
    }
    else if (Keyboard::IsKeyPressed(GLFW_KEY_LEFT) && m_Player->GetDirection() != GameConstants::DIRECTION_RIGHT)
    {
        m_Player->SetDirection(GameConstants::DIRECTION_LEFT);
    }
    else if (Keyboard::IsKeyPressed(GLFW_KEY_RIGHT) && m_Player->GetDirection() != GameConstants::DIRECTION_LEFT)
    {
        m_Player->SetDirection(GameConstants::DIRECTION_RIGHT);
    }
}

bool Game::CheckGameOver() const
{
    return CheckWallCollision() || m_Player->CheckSelfCollision();
}

bool Game::CheckFoodCollision() const
{
    return m_Player->GetGridPosition() == m_food->GetGridPosition();
}

bool Game::CheckWallCollision() const
{
    // Check if the player has collided with the walls using Grid's boundary checking
    return !m_Grid->IsInBounds(m_Player->GetGridPosition());
}

void Game::Shutdown()
{
    std::cout << "Game shutting down." << std::endl;
    m_Player->Destroy();
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
    m_Grid = std::make_unique<Grid>(gridPosition, GameConstants::BACKGROUND_COLOR, cellSize, GameConstants::GRID_CELL_COUNT);
    m_Grid->Initialize();
}

void Game::InitializePlayer()
{
    m_Player = std::make_unique<Player>(
        m_Grid.get(),
        Vec2(GameConstants::INITIAL_TAIL_LENGTH, 0),
        GameConstants::PLAYER_SIZE,
        GameConstants::PLAYER_COLOR,
        GameConstants::DIRECTION_RIGHT,
        GameConstants::INITIAL_TAIL_LENGTH,
        GameConstants::MOVE_SPEED);
    m_Player->Initialize();
}

void Game::InitializeFood()
{
    // Place initial food at center of grid
    Vec2 cellCount = m_Grid->GetCellCount();
    int foodX = static_cast<int>(cellCount.x / 2);
    int foodY = static_cast<int>(cellCount.y / 2);
    Vec2 foodPosition = Vec2{foodX, foodY};
    m_food = std::make_unique<GridTile>(m_Grid.get(), foodPosition, GameConstants::FOOD_SIZE, GameConstants::FOOD_COLOR);
}

bool Game::IsValidFoodPosition(const Vec2 &position) const
{
    // Check if position doesn't overlap with player or tail segments
    if (position == m_Player->GetGridPosition())
        return false;

    for (const GridTile &segment : m_Player->GetTailSegments())
    {
        if (position == segment.GetGridPosition())
            return false;
    }

    return true;
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