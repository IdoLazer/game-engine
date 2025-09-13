#pragma once

#include <Engine.h>
#include "Grid.h"
#include "Timer.h"

class Player
{
private:
    Vec2 m_position;
    Vec2 m_size;
    Color m_color;
    Vec2 m_direction;
    std::vector<Vec2> m_tailSegments;
    float m_moveSpeed;
    Timer m_moveTimer;
    bool m_updateMoveThisFrame{false};
    Grid *m_grid;
    bool m_growTailOnNextMove{false};

public:
    Player() = default;

    void Initialize(
        Grid *grid,
        const Vec2 &startPos,
        const Vec2 &size,
        const Color &color,
        const Vec2 &startDir,
        int initialTailLength,
        float moveSpeed);

    void Destroy();

    void Update(float deltaTime);

    void Render();

    void Move();

    void Grow();

    bool CheckSelfCollision() const;

    // Getters
    Vec2 GetPosition() const { return m_position; }
    const std::vector<Vec2> &GetTailSegments() const { return m_tailSegments; }
    Vec2 GetDirection() const { return m_direction; }

    // Setters
    void SetDirection(const Vec2 &dir);
};
