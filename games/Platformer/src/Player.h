#pragma once

#include <Engine.h>
#include "StateMachine/PlayerStateMachine.h"

// --- Forward Declarations ---
class PlatformerWorld;
class Falcon;

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
    bool IsJumping() const;
    void Jump();
    void StopJump();
    void Glide();
    void StopGlide();
    Engine::EventSubscriber<int> &OnNextLevel() { return m_nextLevelEvent; }
    Engine::EventSubscriber<int> &OnPreviousLevel() { return m_previousLevelEvent; }
    Engine::EventSubscriber<> &OnReloadLevel() { return m_reloadLevelEvent; }

// --- Physics & Collision ---
    // Shared primitives the movement states compose - the math is the same
    // regardless of mode, only which coefficients apply differs per state.
private:
    void ApplyGravity(float deltaTime, float maxSpeed, float scale = 1.0f);
    void ApplyHorizontalAcceleration(float deltaTime, float accCoeff, float decCoeff);
    void HandleCollisions(float deltaTime);
    void MoveAndSlide(Engine::Vec2 &position, float deltaTime);
    void UpdateGroundedState(const Engine::Vec2 &position);
    void UpdateWallContact(const Engine::Vec2 &position);
    void CheckChangeLevel(const Engine::Vec2 &position);

// --- State Transitions ---
private:
    void ChangeGroundedState(bool grounded);
    void ChangeWallState(bool onWall, int direction);
    void EnterWallJump();
    void ClearWallJumpTracking();
    void ClearJumpState();

// --- Movement States ---
    // Enter/Exit are still mostly stubs - see FUTURE.md / the next migration
    // step, which replaces ChangeGroundedState/ChangeWallState/etc.'s direct
    // bool mutation with real TransitionTo calls.
private:
    class GroundedState : public PlayerState
    {
    public:
        explicit GroundedState(Player &player) : m_player(player) {}
        void Enter() override;
        void Exit() override;
        void Update(float deltaTime) override;
        const char *GetName() const override { return "Grounded"; }
    private:
        Player &m_player;
    };

    class AirborneState : public PlayerState
    {
    public:
        explicit AirborneState(Player &player) : m_player(player) {}
        void Enter() override;
        void Exit() override;
        void Update(float deltaTime) override;
        const char *GetName() const override { return "Airborne"; }
    private:
        Player &m_player;
    };

    class OnWallState : public PlayerState
    {
    public:
        explicit OnWallState(Player &player) : m_player(player) {}
        void Enter() override;
        void Exit() override;
        void Update(float deltaTime) override;
        const char *GetName() const override { return "OnWall"; }
    private:
        Player &m_player;
    };

    class GlidingState : public PlayerState
    {
    public:
        explicit GlidingState(Player &player) : m_player(player) {}
        void Enter() override;
        void Exit() override;
        void Update(float deltaTime) override;
        const char *GetName() const override { return "Gliding"; }
    private:
        Player &m_player;
    };

    // Ballistic, no-horizontal-control phase right after a wall jump. Ends by
    // timer, by landing (ClearWallJumpTracking already clears the lock when
    // ChangeGroundedState(true) fires), or - once wired in a later step - by
    // touching the opposite wall.
    class WallJumpLockState : public PlayerState
    {
    public:
        explicit WallJumpLockState(Player &player) : m_player(player) {}
        void Enter() override;
        void Exit() override;
        void Update(float deltaTime) override;
        const char *GetName() const override { return "WallJumpLock"; }
    private:
        Player &m_player;
    };

    PlayerStateMachine m_stateMachine;

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

// --- Movement State ---
private:
    Engine::Vec2 m_velocity{};
    Engine::Vec2 m_direction{};  // Current input direction from player
    bool m_isGrounded{false};
    bool m_isJumping{false};     // True from jump initiation until apex or landing
    bool m_isGliding{false};     // True when holding jump after apex, false when releasing or landing

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

    // Used to prevent consecutive wall jumps from the same wall to climb it
    int m_lastWallJumpDirection{0};
    float m_lastWallJumpHeight{0.0f};

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
    Engine::CommandQueue m_startGlideCommandQueue;

// --- Other ---
private:
    PlatformerWorld *m_world{nullptr};
    Falcon *m_falcon{nullptr};
    Engine::Vec2 m_halfExtents{};
    Engine::Event<int> m_nextLevelEvent;
    Engine::Event<int> m_previousLevelEvent;
    Engine::Event<> m_reloadLevelEvent;
};
