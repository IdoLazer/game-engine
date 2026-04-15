#include "Game.h"
#include <algorithm>
#include <iostream>
#include "GameConstants.h"

using namespace Engine;

// --- Game Interface ---

WindowConfig Game::GetWindowConfig() const
{
    WindowConfig config;

    config.title = "Game";
    config.width = GameConstants::WINDOW_WIDTH;
    config.height = GameConstants::WINDOW_HEIGHT;
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

    m_player->Update(deltaTime);
    ReadInput();
    if (CheckGameOver())
    {
        std::cout << "Game Over!" << std::endl;
        Close();
    }
    else if (CheckFoodCollision())
    {
        PlaceFood();
        m_player->Grow();
    }
}

void Game::Render() const
{
    // Draw background
    Renderer2D::DrawTile(Vec2{0, 0}, m_gridWorldSize, m_backgroundColor);

    // Draw Player
    m_player->Render();

    // Draw Food
    m_food->Render();
}

// --- Game Logic ---

void Game::PlaceFood()
{
    // Randomly place food within grid, ensuring it doesn't overlap with the player or tail
    const int maxAttempts = 100; // Safety limit to prevent infinite loops
    int attempts = 0;
    Vec2 foodPosition;
    do
    {
        Vec2 cellCount = m_grid.GetCellCount();
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
    return CheckWallCollision() || m_player->CheckSelfCollision();
}

bool Game::CheckFoodCollision() const
{
    return m_player->GetGridPosition() == m_food->GetGridPosition();
}

bool Game::CheckWallCollision() const
{
    // Check if the player has collided with the walls using Grid's boundary checking
    return !m_grid.IsInBounds(m_player->GetGridPosition());
}

void Game::Shutdown()
{
    std::cout << "Game shutting down." << std::endl;
    m_player->Destroy();
}

// --- Initialization ---

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

    m_grid = Grid(cellSize, GameConstants::GRID_CELL_COUNT);
    m_gridWorldSize = Vec2(cellSize * GameConstants::GRID_CELL_COUNT.x,
                           cellSize * GameConstants::GRID_CELL_COUNT.y);
    m_backgroundColor = GameConstants::BACKGROUND_COLOR;
}

void Game::InitializePlayer()
{
    m_player = GetScene()->Instantiate<Player>();
    m_player->SetGrid(&m_grid);
    m_player->SetGridPosition(Vec2(GameConstants::INITIAL_TAIL_LENGTH, 0));
    m_player->SetGridSize(GameConstants::PLAYER_SIZE);
    m_player->SetColor(GameConstants::PLAYER_COLOR);
    m_player->SetDirection(GameConstants::DIRECTION_RIGHT);
    m_player->SetInitialTailLength(GameConstants::INITIAL_TAIL_LENGTH);
    m_player->SetMoveSpeed(GameConstants::MOVE_SPEED);
    m_player->Initialize();

    // Initialize Input Manager
    m_inputManager = std::make_unique<InputManager>(m_player);

    // Connect the input manager to the player for command processing
    m_player->SetInputManager(m_inputManager.get());
}

void Game::InitializeFood()
{
    // Place initial food at center of grid
    Vec2 cellCount = m_grid.GetCellCount();
    int foodX = static_cast<int>(cellCount.x / 2);
    int foodY = static_cast<int>(cellCount.y / 2);
    Vec2 foodPosition = Vec2{foodX, foodY};

    m_food = GetScene()->Instantiate<GridTile>();
    m_food->SetGrid(&m_grid);
    m_food->SetGridPosition(foodPosition);
    m_food->SetGridSize(GameConstants::FOOD_SIZE);
    m_food->SetColor(GameConstants::FOOD_COLOR);
    m_food->Initialize();
}

bool Game::IsValidFoodPosition(const Vec2 &position) const
{
    // Check if position doesn't overlap with player or tail segments
    if (position == m_player->GetGridPosition())
        return false;

    for (const GridTile &segment : m_player->GetTailSegments())
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