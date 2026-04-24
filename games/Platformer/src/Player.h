#pragma once

#include <Engine.h>

// --- Forward Declarations ---
class PlatformerInputManager;

class Player : public Engine::GridEntity
{
    DECLARE_TYPE(Player, GridEntity)

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
    void SetDirection(const Engine::Vec2 &dir);
    bool IsJumping() const;
    void Jump();
    void StopJump();

// --- Internal Methods ---
private:
    void ApplyGravity(float deltaTime);
    void HandleCollisions(float deltaTime);
    void ChangeGroundedState(bool grounded);

// --- Fields ---
private:
    float m_speed{0.0f};
    float m_jumpForce{0.0f};
    float m_gravity{0.0f};
    float m_coyoteTime{0.0f};
    float m_jumpBufferTime{0.0f};
    float m_minJumpTime{0.0f};
    bool m_isJumping{false};
    bool m_isGrounded{false};
    bool m_inCoyoteTime{false};
    bool m_inMinJump{false};
    Engine::Vec2 m_playerBoundingBox[2]{};
    Engine::Vec2 m_velocity{};
    Engine::Vec2 m_direction{};
    Engine::Timer m_coyoteTimer;
    Engine::Timer m_jumpBufferTimer;
    Engine::Timer m_minJumpTimer;
    Engine::CommandQueue m_jumpCommandQueue;
    Engine::CommandQueue m_jumpStopCommandQueue;
    std::unique_ptr<PlatformerInputManager> m_inputManager;
};