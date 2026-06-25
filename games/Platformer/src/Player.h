#pragma once

#include <Engine.h>

// --- Forward Declarations ---
class PlatformerWorld;

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
    bool IsJumping() const;
    void Jump();
    void StopJump();
    Engine::EventSubscriber<int> &OnNextLevel() { return m_nextLevelEvent; }
    Engine::EventSubscriber<int> &OnPreviousLevel() { return m_previousLevelEvent; }
    Engine::EventSubscriber<> &OnReloadLevel() { return m_reloadLevelEvent; }

// --- Physics & Collision ---
private:
    void ApplyGravity(float deltaTime);
    void ApplyHorizontalMovement(float deltaTime);
    void HandleCollisions(float deltaTime);
    void ResolveHorizontalCollisions(const Engine::Vec2 &currentPos, Engine::Vec2 &newGridPos);
    void ResolveVerticalCollisions(const Engine::Vec2 &currentPos, Engine::Vec2 &newGridPos);
    void UpdateWallContact(const Engine::Vec2 &position);
    void CheckChangeLevel(const Engine::Vec2 &position);

// --- State Transitions ---
private:
    void ChangeGroundedState(bool grounded);
    void ChangeWallState(bool onWall, int direction);
    void EnterWallJump();
    void ClearWallJumpTracking();
    void ClearJumpState();

// --- Configuration (data-driven via type registry) ---
private:
    float m_speed{0.0f};
    float m_accCoeff{0.0f};
    float m_decCoeff{0.0f};
    float m_airAccCoeff{0.0f};
    float m_airDecCoeff{0.0f};
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

// --- Movement State ---
private:
    Engine::Vec2 m_velocity{};
    Engine::Vec2 m_direction{};  // Current input direction from player
    bool m_isGrounded{false};
    bool m_isJumping{false};     // True from jump initiation until apex or landing

// --- Wall State ---
private:
    bool m_isOnWall{false};
    bool m_isWallSliding{false};
    int m_wallDirection{0};      // -1 = wall on left, 1 = wall on right
    int m_lastWallDirection{0};  // remembered during wall coyote time

// --- Wall Jump State ---
    // After a wall jump, the player enters a "lock" phase (ballistic arc, no input),
    // followed by a "coasting" phase (maintains velocity, no deceleration until input).
private:
    bool m_inWallJumpLock{false};
    bool m_wallJumpCoasting{false};

// --- Jump Assist State ---
    // Coyote time: brief window after leaving a ledge/wall where jump is still allowed.
    // Jump buffer: if jump is pressed just before landing, it fires on contact.
    // Min jump: ensures a minimum arc height even on quick tap.
private:
    bool m_inCoyoteTime{false};
    bool m_inWallCoyoteTime{false};
    bool m_inMinJump{false};

// --- Timers ---
private:
    Engine::Timer m_coyoteTimer;
    Engine::Timer m_wallCoyoteTimer;
    Engine::Timer m_jumpBufferTimer;
    Engine::Timer m_minJumpTimer;
    Engine::Timer m_wallJumpLockTimer;

// --- Command Queues ---
private:
    Engine::CommandQueue m_jumpCommandQueue;
    Engine::CommandQueue m_jumpStopCommandQueue;

// --- Other ---
private:
    PlatformerWorld *m_world{nullptr};
    Engine::Vec2 m_playerBoundingBox[2]{};
    Engine::Event<int> m_nextLevelEvent;
    Engine::Event<int> m_previousLevelEvent;
    Engine::Event<> m_reloadLevelEvent;
};
