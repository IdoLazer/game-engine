#pragma once

#include <Engine.h>
#include "Commands/MovementCommands.h"

class Player : public Engine::GridEntity, public IMovable
{
private:
    Engine::Vec2 m_direction;
    std::vector<Engine::GridTile> m_tailSegments;
    float m_moveSpeed;
    Engine::Timer m_moveTimer;
    bool m_updateMoveThisFrame{false};
    bool m_growTailOnNextMove{false};

    // Input manager reference for command processing
    class InputManager *m_inputManager{nullptr};

public:
    Player(const Engine::Grid *grid,
           const Engine::Vec2 &startPos,
           const Engine::Vec2 &size,
           const Engine::Color &color,
           const Engine::Vec2 &startDir,
           int initialTailLength,
           float moveSpeed)
        : Engine::GridEntity(grid, startPos, size, color),
          m_direction(startDir), m_moveSpeed(moveSpeed)
    {
        m_tailSegments.reserve(initialTailLength);
    }
    ~Player() = default;

    // Entity interface implementation
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() const override;

    void Destroy();
    void Move();
    void Grow();
    bool CheckSelfCollision() const;

    const std::vector<Engine::GridTile> &GetTailSegments() const { return m_tailSegments; }
    Engine::Vec2 GetDirection() const { return m_direction; }

    // Setters
    void SetDirection(const Engine::Vec2 &dir) override;
    void SetInputManager(class InputManager *inputManager) { m_inputManager = inputManager; }

private:
    // Command processing - executes next queued command if available
    void ProcessQueuedCommand();
};
