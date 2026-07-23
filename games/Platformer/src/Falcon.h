#pragma once

#include <Engine.h>
#include <optional>

// --- Forward Declarations ---
class Player;
class PlatformerWorld;
class Cursor;

enum class FalconState
{
    OnShoulder,  // Resting on the player's shoulder, following them
    Flying,      // Moving out towards a latch point after being released
    Returning,   // Flying back to the player after Retrieve()
    Latched      // Stuck to a ceiling tile, acting as a fixed hinge point
};

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

// --- Falcon Behavior ---
private:
    void SetAimPoint(const Engine::Vec2 &aimPoint);
    bool FlyTowards(const Engine::Vec2 &target, float deltaTime);
    void LatchToCeiling();

// --- Configuration (data-driven via type registry) ---
private:
    Engine::Vec2 m_offsetFromPlayer{0.0f, 0.0f}; // Falcon's position relative to the player
    float m_speed{0.0f}; // Falcon's movement speed
    float m_snapRadius{0.0f}; // Distance to a flight target within which it's considered reached

// --- Private Fields ---
private:
    Player *m_player{nullptr};
    PlatformerWorld *m_world{nullptr};
    Cursor *m_cursor{nullptr};
    Engine::Vec2 m_direction{1.0f, 0.0f}; // Direction the falcon is facing
    FalconState m_state{FalconState::OnShoulder}; // The falcon's current behavior state
    Engine::Vec2 m_aimPoint{0.0f, 0.0f}; // The point the falcon is currently aiming at
    std::optional<Engine::Vec2> m_latchPoint; // The ceiling point to fly to, if the current aim point is a valid latch target
    bool m_isAiming{false}; // Whether the falcon is currently aiming at the cursor
};
