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
    void StartAiming();
    void ReleaseAiming();
    void Retrieve();

// --- Falcon Behavior ---
private:
    void SetGoal(const Engine::Vec2 &goal);
    void MoveToGoal(float deltaTime);
    void LatchToCeiling();
    void ReturnToPlayer(float deltaTime);

// --- Configuration (data-driven via type registry) ---
private:
    Engine::Vec2 m_offsetFromPlayer{0.0f, 0.0f}; // Falcon's position relative to the player
    float m_speed{0.0f}; // Falcon's movement speed

// --- Private Fields ---
private:
    Player *m_player{nullptr};
    PlatformerWorld *m_world{nullptr};
    Cursor *m_cursor{nullptr};
    Engine::Vec2 m_direction{1.0f, 0.0f}; // Direction the falcon is facing
    bool m_isFollowingPlayer{true}; // Whether the falcon is following the player or acting independently
    bool m_isMovingToGoal{false}; // Whether the falcon is currently moving towards a goal position
    bool m_isReturningToPlayer{false}; // Whether the falcon is returning to the player after reaching a goal
    Engine::Vec2 m_goal{0.0f, 0.0f}; // The goal position the falcon should move towards
    Engine::Vec2 m_latchPoint{0.0f, 0.0f}; // The point where the falcon has latched onto a ceiling tile
    bool m_isAiming{false}; // Whether the falcon is currently aiming at the cursor
    bool m_isGoalLatchable{false}; // Whether the goal is a solid tile being aimed at from below (a valid ceiling-latch target)
    bool m_isLatched{false}; // Whether the falcon has latched onto a ceiling tile and become a hinge point
};
