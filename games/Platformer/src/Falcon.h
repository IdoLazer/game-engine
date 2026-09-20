#pragma once

#include <Engine.h>
#include "StateMachine/FalconStateMachine.h"
#include <optional>

// --- Forward Declarations ---
class Player;
class PlatformerWorld;
class Cursor;

// The body: position, flight, aiming and drawing primitives. Which to apply,
// and when, belongs to the states in StateMachine/FalconStates.h.
class Falcon : public Engine::GridEntity
{
    DECLARE_TYPE(Falcon, GridEntity)

// --- Constructors & Destructors ---
public:
    Falcon() = default;
    ~Falcon() = default;

// --- Lifecycle ---
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() const override;

// --- Public Interface ---
public:
    void SetPlayer(Player *player) { m_player = player; }
    void SetWorld(PlatformerWorld *world) { m_world = world; }
    void SetCursor(Cursor *cursor) { m_cursor = cursor; }
    void StartAiming();
    void ReleaseAiming();
    void Retrieve();
    bool IsOnShoulder() const;
    void StartGlide();
    void StopGlide();
    Engine::EventSubscriber<> &OnReturned() { return m_onReturnedEvent; }

// --- Behavior States ---
    // Defined in StateMachine/FalconStates.h. Nested so they can reach the
    // primitives below without any of it becoming public.
private:
    // Their shared base isn't nested, but its default rendering needs the same access.
    friend class FalconState;

    class OnShoulderState;
    class AimingState;
    class FlyingState;
    class ReturningState;
    class LatchedState;
    class GlidingState;

    template <typename TState>
    void RegisterState(std::optional<FalconStateId> parent = std::nullopt)
    {
        m_stateMachine.RegisterState(TState::Id, std::make_unique<TState>(*this, m_stateMachine), parent);
    }

    FalconStateMachine m_stateMachine;

// --- Flight & Aiming ---
private:
    Engine::Vec2 ShoulderPosition() const;
    void FollowPlayer(const Engine::Vec2 &offset);
    void AimAtCursor();
    void SetAimPoint(const Engine::Vec2 &aimPoint);
    bool FlyTowards(const Engine::Vec2 &target, float speed, float deltaTime);

// --- Deferred Aim ---
private:
    void QueueAimRequest();
    void ConsumeQueuedAimRequest();
    void ClearQueuedAimRequest();

// --- Rendering ---
private:
    void DrawBody() const;
    void DrawGlidingBody() const;
    void DrawAimOverlay() const;

// --- Configuration (data-driven via type registry) ---
private:
    Engine::Vec2 m_offsetFromPlayer{0.0f, 0.0f}; // Falcon's position relative to the player
    Engine::Vec2 m_glideOffsetFromPlayer{0.0f, 0.0f}; // Falcon's position relative to the player while gliding
    float m_speed{0.0f}; // Falcon's movement speed
    float m_retrieveSpeed{0.0f}; // Falcon's movement speed when returning to the player
    float m_snapRadius{0.0f}; // Distance to a flight target within which it's considered reached

// --- Private Fields ---
private:
    Player *m_player{nullptr};
    PlatformerWorld *m_world{nullptr};
    Cursor *m_cursor{nullptr};
    Engine::Vec2 m_direction{1.0f, 0.0f}; // Direction the falcon is facing
    Engine::Vec2 m_aimPoint{0.0f, 0.0f}; // The point the falcon is currently aiming at
    std::optional<Engine::Vec2> m_latchPoint; // The point to fly to, if the current aim point is a valid latch target
    Engine::Vec2 m_latchDirection{1.0f, 0.0f}; // Direction to face once latched - embeds into the surface, opposite its hit normal
    Engine::CommandQueue m_startAimingCommandQueue; // Queued command to start aiming, executed when the falcon is on the shoulder
    Engine::Event<> m_onReturnedEvent; // Notifies the player when the falcon has returned to its shoulder
};
