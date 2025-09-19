#pragma once

#include <Engine.h>

using namespace Engine;

class Player : public GridEntity
{
private:
    Vec2 m_direction;
    std::vector<GridTile> m_tailSegments;
    float m_moveSpeed;
    Timer m_moveTimer;
    bool m_updateMoveThisFrame{false};
    bool m_growTailOnNextMove{false};

public:
    Player(const Grid *grid,
           const Vec2 &startPos,
           const Vec2 &size,
           const Color &color,
           const Vec2 &startDir,
           int initialTailLength,
           float moveSpeed)
        : GridEntity(grid, startPos, size, color),
          m_direction(startDir), m_moveSpeed(moveSpeed)
    {
        m_tailSegments.reserve(initialTailLength);
    }
    ~Player() = default;

    // Entity interface implementation
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;

    void Destroy();
    void Move();
    void Grow();
    bool CheckSelfCollision() const;

    const std::vector<GridTile> &GetTailSegments() const { return m_tailSegments; }
    Vec2 GetDirection() const { return m_direction; }

    // Setters
    void SetDirection(const Vec2 &dir);
};
