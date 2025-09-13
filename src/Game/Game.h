#pragma once
#include <Engine.h> // Elegant system header syntax!
#include <vector>
#include <functional>

using namespace Engine;

// Game Constants
namespace GameConstants
{
    // Player Settings
    const Vec2 PLAYER_SIZE{1.0f, 1.0f};
    const Color PLAYER_COLOR{1.0f, 0.0f, 0.0f, 1.0f}; // Red
    static constexpr int INITIAL_TAIL_LENGTH = 1;
    static constexpr float MOVE_SPEED = 6.0f; // Grid cells per second

    // Food Settings
    const Vec2 FOOD_SIZE{0.5f, 0.5f};
    const Color FOOD_COLOR{0.0f, 1.0f, 0.0f, 1.0f}; // Green

    // World Settings
    const Vec2 GRID_CELL_COUNT{13.0f, 10.0f};
    const Color BACKGROUND_COLOR{0.0f, 0.0f, 1.0f, 1.0f}; // Blue

    // Movement
    const Vec2 DIRECTION_UP{0.0f, -1.0f};
    const Vec2 DIRECTION_DOWN{0.0f, 1.0f};
    const Vec2 DIRECTION_LEFT{-1.0f, 0.0f};
    const Vec2 DIRECTION_RIGHT{1.0f, 0.0f};
}

// Simple timer utility for game timing
class Timer
{
public:
    using Callback = std::function<void()>;

private:
    float m_interval;
    float m_accumulator;
    Callback m_callback;
    bool m_isLooping;
    bool m_isActive;

public:
    Timer(float interval, Callback callback, bool isLooping = true)
        : m_interval(interval), m_accumulator(0.0f), m_callback(callback),
          m_isLooping(isLooping), m_isActive(true) {}

    // Update the timer and execute callback if needed
    void Update(float deltaTime)
    {
        if (!m_isActive)
            return;

        m_accumulator += deltaTime;
        if (m_accumulator >= m_interval)
        {
            if (m_callback)
            {
                m_callback();
            }

            if (m_isLooping)
            {
                m_accumulator = 0.0f;
            }
            else
            {
                m_isActive = false; // One-shot timer stops after first execution
            }
        }
    }

    // Control methods
    void Reset()
    {
        m_accumulator = 0.0f;
        m_isActive = true;
    }
    void Stop() { m_isActive = false; }
    void Start() { m_isActive = true; }
    void SetInterval(float interval) { m_interval = interval; }
    void SetCallback(Callback callback) { m_callback = callback; }
    bool IsActive() const { return m_isActive; }
};

// Grid coordinate system - simple grid entity with configurable dimensions
class Grid
{
private:
    Vec2 m_position;  // World position of grid center
    Vec2 m_size;      // Total size of grid in world units
    Vec2 m_cellCount; // Number of cells (width x height)
    float m_cellSize; // Size of each cell in world units (assuming square cells)
    Vec2 m_topLeft;   // Cached top-left position for conversions

public:
    Grid() = default;

    Grid(Vec2 position, float cellSize, Vec2 cellCount)
        : m_position(position), m_cellSize(cellSize), m_cellCount(cellCount)
    {
        // Calculate total world size from cell size and count
        m_size = Vec2(m_cellSize * m_cellCount.x, m_cellSize * m_cellCount.y);
        m_topLeft = Vec2(m_position.x - m_size.x * 0.5f, m_position.y + m_size.y * 0.5f);
    }

    void Initialize(Vec2 position, float cellSize, Vec2 cellCount)
    {
        m_position = position;
        m_cellSize = cellSize;
        m_cellCount = cellCount;
        m_size = Vec2(m_cellSize * m_cellCount.x, m_cellSize * m_cellCount.y);
        m_topLeft = Vec2(m_position.x - m_size.x * 0.5f, m_position.y + m_size.y * 0.5f);
    }

    Vec2 GridToWorld(Vec2 gridPos) const
    {
        // Grid (0,0) is top-left for game convenience
        // World Y increases upward, but grid Y increases downward
        float worldX = m_topLeft.x + (gridPos.x + 0.5f) * m_cellSize;
        float worldY = m_topLeft.y - (gridPos.y + 0.5f) * m_cellSize;
        return Vec2(worldX, worldY);
    }

    // Convert world position to grid coordinates
    Vec2 WorldToGrid(Vec2 worldPos) const
    {
        float gridX = (worldPos.x - m_topLeft.x) / m_cellSize;
        float gridY = (m_topLeft.y - worldPos.y) / m_cellSize;
        return Vec2(gridX, gridY);
    }

    // Check if grid position is within bounds
    bool IsInBounds(Vec2 gridPos) const
    {
        return gridPos.x >= 0 && gridPos.x < m_cellCount.x &&
               gridPos.y >= 0 && gridPos.y < m_cellCount.y;
    }

    // Getters
    Vec2 GetPosition() const { return m_position; }
    Vec2 GetSize() const { return m_size; }
    Vec2 GetCellCount() const { return m_cellCount; }
    float GetCellSize() const { return m_cellSize; }
};

class Game : public Engine::Application
{
private:
    void ReadInput();
    void MovePlayer();
    bool CheckGameOver() const;
    bool CheckFoodCollision() const;
    bool CheckWallCollision() const;
    bool CheckSelfCollision() const;
    void GrowPlayer();
    void PlaceFood();

    // Timer callbacks
    void OnMoveTimer();

    // Initialization helpers
    void InitializeWorld();
    void InitializePlayer();
    void InitializeFood();

    // Validation helpers
    bool IsValidFoodPosition(const Vec2 &position) const;

public:
    Game() = default;
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Shutdown() override;

    WindowConfig GetWindowConfig() const override;

private:
    // World
    Grid m_Grid; // Our grid coordinate system

    // Player
    Vec2 m_PlayerCell; // Start at top-left corner
    Vec2 m_MoveDirection;
    std::vector<Vec2> m_TailSegments;
    Timer m_MoveTimer{1.0f / GameConstants::MOVE_SPEED, [this]()
                      { OnMoveTimer(); }, true};
    bool m_UpdateMoveThisFrame{false};

    // Food
    Vec2 m_FoodCell;
};