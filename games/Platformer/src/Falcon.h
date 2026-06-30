#pragma once

#include <Engine.h>

// --- Forward Declarations ---
class Player;
class PlatformerWorld;
class Cursor;

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
    void StartAiming() { m_isAiming = true; }
    void ReleaseAiming() { m_isAiming = false; }

// --- Falcon Behavior ---
private:
    void SetGoal(const Engine::Vec2 &goal);

// --- Configuration (data-driven via type registry) ---
private:
    Engine::Vec2 m_offsetFromPlayer{0.0f, 0.0f}; // Falcon's position relative to the player

// --- Private Fields ---
private:
    Player *m_player{nullptr};
    PlatformerWorld *m_world{nullptr};
    Cursor *m_cursor{nullptr};
    Engine::Vec2 m_direction{1.0f, 0.0f}; // Direction the falcon is facing
    bool m_isFollowingPlayer{true}; // Whether the falcon is following the player or acting independently
    Engine::Vec2 m_goal{0.0f, 0.0f}; // The goal position the falcon should move towards
    bool m_isAiming{false}; // Whether the falcon is currently aiming at the cursor
};