#pragma once

#include <Engine.h>
#include "StateMachine/PlayerStateMachine.h"

// --- Forward Declarations ---
class PlatformerWorld;
class Falcon;

// The body: velocity, collision, and the physics primitives that act on them.
// Every decision about which of those to apply, and when, belongs to the
// movement states in StateMachine/PlayerStates.h.
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
    void Destroy() override;

// --- Public Interface ---
public:
    void SetDirection(const Engine::Vec2 &dir);
    void SetWorld(PlatformerWorld *world);
    void SetFalcon(Falcon *falcon);
    void Jump();
    void StopJump();
    void Glide();
    void StopGlide();
    Engine::EventSubscriber<int> &OnNextLevel() { return m_nextLevelEvent; }
    Engine::EventSubscriber<int> &OnPreviousLevel() { return m_previousLevelEvent; }
    Engine::EventSubscriber<> &OnReloadLevel() { return m_reloadLevelEvent; }

// --- Movement States ---
    // Defined in StateMachine/PlayerStates.h. Nested so they can reach the
    // physics below without any of it becoming public.
private:
    class GroundedState;
    class OnWallState;
    class AirborneState;
    class AirborneSubState;
    class JumpingState;
    class FallingState;
    class GlidingState;

    template <typename TState>
    void RegisterState(std::optional<PlayerStateId> parent = std::nullopt)
    {
        m_stateMachine.RegisterState(TState::Id, std::make_unique<TState>(*this, m_stateMachine), parent);
    }

    PlayerStateMachine m_stateMachine;

// --- Physics & Collision ---
    // Primitives the states compose - the math is the same whichever state is
    // active, only the coefficients handed to it differ.
private:
    void ApplyGravity(float deltaTime, float maxSpeed, float scale = 1.0f);
    void ApplyHorizontalAcceleration(float deltaTime, float accCoeff, float decCoeff);
    void LaunchJump();
    void LaunchWallJump(int wallDirection);
    void StopAgainstWall(int wallDirection);
    void ForgetWallJumpHistory();
    void HandleCollisions(float deltaTime);
    void MoveAndSlide(Engine::Vec2 &position, float deltaTime);
    PlayerContacts ProbeContacts(const Engine::Vec2 &position) const;
    void CheckChangeLevel(const Engine::Vec2 &position);
    bool CanGlide() const;

// --- Jump Buffer ---
    // A jump pressed too early to act on, replayed on the next landing or wall grab.
private:
    void BufferJump();
    void ConsumeBufferedJump();
    void ClearBufferedJump();

// --- Configuration (data-driven via type registry) ---
private:
    float m_speed{0.0f};
    float m_accCoeff{0.0f};
    float m_decCoeff{0.0f};
    float m_airAccCoeff{0.0f};
    float m_airDecCoeff{0.0f};
    float m_glideAccCoeff{0.0f};
    float m_glideDecCoeff{0.0f};
    float m_wallHitDecCoeff{0.0f};
    float m_jumpForce{0.0f};
    float m_gravity{0.0f};
    float m_coyoteTime{0.0f};
    float m_wallCoyoteTime{0.0f};
    float m_jumpBufferTime{0.0f};
    float m_minJumpTime{0.0f};
    float m_wallJumpLockTime{0.0f};
    float m_wallJumpForce{0.0f};
    float m_wallJumpAngle{0.0f}; // Degrees from horizontal (0 = pure sideways, 90 = pure up)
    float m_maxFallSpeed{0.0f};
    float m_wallSlideMaxSpeed{0.0f};
    float m_wallGravityScale{1.0f};
    float m_glideMaxSpeed{0.0f};
    float m_glideGravityScale{1.0f};

// --- Movement ---
private:
    Engine::Vec2 m_velocity{};
    Engine::Vec2 m_direction{}; // Current input direction from player

// --- Wall Jump History ---
    // Where the last wall jump launched from, cleared on landing. Outlives the
    // states involved, which is what stops the same wall being climbed by
    // jumping off it over and over.
private:
    int m_lastWallJumpDirection{0};
    float m_lastWallJumpHeight{0.0f};

// --- Other ---
private:
    Engine::CommandQueue m_jumpCommandQueue;
    Engine::Timer m_jumpBufferTimer;
    PlatformerWorld *m_world{nullptr};
    Falcon *m_falcon{nullptr};
    Engine::Vec2 m_halfExtents{};
    Engine::Event<int> m_nextLevelEvent;
    Engine::Event<int> m_previousLevelEvent;
    Engine::Event<> m_reloadLevelEvent;
};
