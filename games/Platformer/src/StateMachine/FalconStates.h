#pragma once

#include "../Falcon.h"
#include <Engine.h>

// The falcon's behavior states. Aiming nests under OnShoulder - the falcon
// rides the shoulder the whole time it aims.

// --- On Shoulder ---

class Falcon::OnShoulderState : public FalconState
{
public:
    static constexpr FalconStateId Id = FalconStateId::OnShoulder;
    using FalconState::FalconState;

    void Enter() override;
    void Update(float deltaTime) override;
    bool OnAimRequested() override;
    bool OnGlideRequested() override;
    const char *GetName() const override { return "OnShoulder"; }
};

// --- Aiming ---

// Tracking the cursor. Holds no aim data of its own - the aim point and latch
// target live on Falcon, since a released aim outlives this state.
class Falcon::AimingState : public FalconState
{
public:
    static constexpr FalconStateId Id = FalconStateId::Aiming;

    // Update only aims; OnShoulder must run first to place the falcon.
    static constexpr FalconStateId RequiredAncestor = FalconStateId::OnShoulder;

    using FalconState::FalconState;

    void Update(float deltaTime) override;
    bool OnAimReleased() override;
    bool OnGlideRequested() override;
    void Render() const override;
    const char *GetName() const override { return "Aiming"; }
};

// --- Flying ---

class Falcon::FlyingState : public FalconState
{
public:
    static constexpr FalconStateId Id = FalconStateId::Flying;
    using FalconState::FalconState;

    void Update(float deltaTime) override;
    bool OnRetrieveRequested() override;
    const char *GetName() const override { return "Flying"; }
};

// --- Returning ---

class Falcon::ReturningState : public FalconState
{
public:
    static constexpr FalconStateId Id = FalconStateId::Returning;
    using FalconState::FalconState;

    void Update(float deltaTime) override;
    const char *GetName() const override { return "Returning"; }
};

// --- Latched ---

class Falcon::LatchedState : public FalconState
{
public:
    static constexpr FalconStateId Id = FalconStateId::Latched;
    using FalconState::FalconState;

    void Enter() override;
    bool OnRetrieveRequested() override;
    const char *GetName() const override { return "Latched"; }
};

// --- Gliding ---

class Falcon::GlidingState : public FalconState
{
public:
    static constexpr FalconStateId Id = FalconStateId::Gliding;
    using FalconState::FalconState;

    void Update(float deltaTime) override;
    bool OnGlideReleased() override;
    void Render() const override;
    const char *GetName() const override { return "Gliding"; }
};
