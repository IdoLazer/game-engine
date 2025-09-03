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
    const Vec2 GRID_CELL_SIZE{1.0f, 1.0f};
    const Color BOUNDARY_COLOR{0.0f, 0.0f, 1.0f, 1.0f}; // Blue

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

    // Grid utility
    Vec2 GridToWorldPosition(int gridX, int gridY, float worldWidth, float worldHeight) const;
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
    Vec2 m_GridSize;
    std::vector<std::vector<Vec2>> m_GridCells;
    float m_MarginX;

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