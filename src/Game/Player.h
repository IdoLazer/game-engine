#pragma once

#include <Engine.h>
#include "Commands/MovementCommands.h"

class Player : public Engine::GridEntity, public IMovable
{
    DECLARE_TYPE(Player, GridEntity)

// --- Fields ---
private:
    Engine::Vec2 m_direction{};
    int m_initialTailLength{0};
    float m_moveSpeed{0.0f};
    std::vector<Engine::GridTile> m_tailSegments;
    Engine::Timer m_moveTimer;
    bool m_updateMoveThisFrame{false};
    bool m_growTailOnNextMove{false};
    class InputManager *m_inputManager{nullptr};

// --- Constructors & Destructors ---
public:
    Player() = default;
    ~Player() = default;

// --- Lifecycle ---
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() const override;
    void Destroy();

// --- Accessors ---
public:
    const std::vector<Engine::GridTile> &GetTailSegments() const;
    Engine::Vec2 GetDirection() const;
    void SetDirection(const Engine::Vec2 &dir) override;
    void SetInputManager(class InputManager *inputManager);
    void SetInitialTailLength(int length);
    void SetMoveSpeed(float speed);

// --- Game Logic ---
public:
    void Move();
    void Grow();
    bool CheckSelfCollision() const;

// --- Internal ---
private:
    void ProcessQueuedCommand();
};
