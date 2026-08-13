#pragma once

#include "../Player.h"
#include <Engine.h>

// Player's movement states. Each one owns the timers and flags that only make
// sense while it is active, handles the inputs it can act on, and clears itself
// on exit. Anything that outlives a single state belongs to its parent.

// --- Grounded ---

class Player::GroundedState : public PlayerState
{
public:
    static constexpr PlayerStateId Id = PlayerStateId::Grounded;
    using PlayerState::PlayerState;

    void Enter() override;
    void Update(float deltaTime) override;
    bool OnJumpPressed() override;
    bool OnContacts(const PlayerContacts &contacts) override;
    const char *GetName() const override { return "Grounded"; }
};

// --- On Wall ---

class Player::OnWallState : public PlayerState
{
public:
    static constexpr PlayerStateId Id = PlayerStateId::OnWall;
    using PlayerState::PlayerState;

    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    bool OnJumpPressed() override;
    bool OnGlidePressed() override;
    bool OnDirectionChanged(const Engine::Vec2 &direction) override;
    bool OnContacts(const PlayerContacts &contacts) override;
    void AdjustVisual(Engine::Vec2 &center, Engine::Vec2 &size) const override;
    const char *GetName() const override { return "OnWall"; }

    // Which wall to attach to, set by whoever transitions us in.
    void GrabWall(int direction) { m_wallDirection = direction; }

private:
    void LetGoOfWall();

    int m_wallDirection{0}; // -1 = wall on left, 1 = wall on right
    bool m_isWallSliding{false};
};

// --- Airborne ---

// Parent of every off-the-ground state. Owns the two modifiers that outlive any
// one of them: the wall jump lock (ballistic, no horizontal control) and the
// coast that follows it (keeps the launch momentum until there's input).
class Player::AirborneState : public PlayerState
{
public:
    static constexpr PlayerStateId Id = PlayerStateId::Airborne;
    AirborneState(Player &player, PlayerStateMachine &machine);

    void Exit() override;
    void Update(float deltaTime) override;
    bool OnJumpPressed() override;
    bool OnGlidePressed() override;
    bool OnGlideReleased() override;
    bool OnContacts(const PlayerContacts &contacts) override;
    const char *GetName() const override { return "Airborne"; }

    // Horizontal control for the substates, gated by the modifiers above.
    void ApplyAirControl(float deltaTime, float accCoeff, float decCoeff);
    void BeginWallJumpLock();
    void StopCoasting() { m_coasting = false; }

private:
    void EndWallJumpLock();

    Engine::Timer m_lockTimer;
    bool m_coasting{false};
    Engine::CommandQueue m_glideCommandQueue; // a glide asked for mid-lock, deferred
};

// Shared base for the substates of AirborneState, giving them typed access to
// the parent that owns what they have in common.
class Player::AirborneSubState : public PlayerState
{
public:
    using PlayerState::PlayerState;

protected:
    AirborneState &Airborne() const { return static_cast<AirborneState &>(*GetParent()); }
};

// --- Jumping ---

// Rising from a jump, which is the only window in which the jump can still be
// cut short. Ends at the apex, or early when the player releases the button.
class Player::JumpingState : public Player::AirborneSubState
{
public:
    static constexpr PlayerStateId Id = PlayerStateId::Jumping;
    JumpingState(Player &player, PlayerStateMachine &machine);

    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    bool OnJumpReleased() override;
    const char *GetName() const override { return "Jumping"; }

private:
    void EndMinimumJump();

    Engine::Timer m_minJumpTimer;             // guarantees a minimum arc on a quick tap
    Engine::CommandQueue m_stopCommandQueue;  // a release during that arc, deferred
};

// --- Falling ---

class Player::FallingState : public Player::AirborneSubState
{
public:
    static constexpr PlayerStateId Id = PlayerStateId::Falling;
    FallingState(Player &player, PlayerStateMachine &machine);

    void Exit() override;
    void Update(float deltaTime) override;
    bool OnJumpPressed() override;
    const char *GetName() const override { return "Falling"; }

    // Coyote time: a brief window in which the jump the player just lost still
    // registers. Granted by whoever transitions us in, since only they know
    // what was left behind.
    void GrantGroundCoyote();
    void GrantWallCoyote(int wallDirection);

private:
    enum class CoyoteJump { None, Ground, Wall };

    Engine::Timer m_coyoteTimer;
    CoyoteJump m_coyoteJump{CoyoteJump::None};
    int m_coyoteWallDirection{0};
};

// --- Gliding ---

class Player::GlidingState : public Player::AirborneSubState
{
public:
    static constexpr PlayerStateId Id = PlayerStateId::Gliding;
    using AirborneSubState::AirborneSubState;

    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    bool OnJumpPressed() override;
    bool OnGlideReleased() override;
    bool OnContacts(const PlayerContacts &contacts) override;
    const char *GetName() const override { return "Gliding"; }
};
