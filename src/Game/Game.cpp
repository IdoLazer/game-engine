#include "Game.h"
#include <algorithm>
#include <iostream>
#include "GameConstants.h"

using namespace Engine;

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
    // Process input and convert to commands
    m_inputManager->ProcessInput();

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

void Game::Render() const
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
    if (m_inputManager->IsExitRequested())
    {
        std::cout << "Escape pressed - exiting game!" << std::endl;
        Close();
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
    m_Grid = GetScene()->Instantiate<Grid>(gridPosition, GameConstants::BACKGROUND_COLOR, cellSize, GameConstants::GRID_CELL_COUNT);
}

void Game::InitializePlayer()
{
    m_Player = GetScene()->Instantiate<Player>(
        m_Grid,
        Vec2(GameConstants::INITIAL_TAIL_LENGTH, 0),
        GameConstants::PLAYER_SIZE,
        GameConstants::PLAYER_COLOR,
        GameConstants::DIRECTION_RIGHT,
        GameConstants::INITIAL_TAIL_LENGTH,
        GameConstants::MOVE_SPEED);

    // Initialize Input Manager
    m_inputManager = std::make_unique<InputManager>(m_Player);

    // Connect the input manager to the player for command processing
    m_Player->SetInputManager(m_inputManager.get());
}

void Game::InitializeFood()
{
    // Place initial food at center of grid
    Vec2 cellCount = m_Grid->GetCellCount();
    int foodX = static_cast<int>(cellCount.x / 2);
    int foodY = static_cast<int>(cellCount.y / 2);
    Vec2 foodPosition = Vec2{foodX, foodY};
    m_food = GetScene()->Instantiate<GridTile>(m_Grid, foodPosition, GameConstants::FOOD_SIZE, GameConstants::FOOD_COLOR);
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