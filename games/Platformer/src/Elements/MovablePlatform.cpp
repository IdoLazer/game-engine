#include "MovablePlatform.h"
#include "../PlatformerWorld.h"

#include <iostream>
#include <optional>

using namespace Engine;

// --- Type Registration ---

BEGIN_TYPE_REGISTER(MovablePlatform)
    REGISTER_PROPERTY(std::string, EndAnchor, &MovablePlatform::m_endAnchor)
    REGISTER_PROPERTY(float, MoveSpeed, &MovablePlatform::m_moveSpeed)
    REGISTER_PROPERTY(bool, SwitchedOn, &MovablePlatform::m_switchedOn)
END_TYPE_REGISTER()

// --- Lifecycle ---

void MovablePlatform::Update(float deltaTime)
{
    Vec2 goal = m_switchedOn ? m_end : m_start;
    Vec2 position = GetGridPosition();
    m_collider.MoveAndPush(position.MoveTowards(goal, m_moveSpeed * deltaTime) - position);
}

void MovablePlatform::Render() const
{
    Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);
}

// --- Public Interface ---

bool MovablePlatform::Place(PlatformerWorld &world)
{
    if (!LevelRegion::Place(world))
        return false;

    m_start = GetGridPosition();
    m_end = m_start;

    if (std::optional<Rect> bounds = world.ClaimAnchor(GetEndAnchor()))
        m_end = bounds->center;
    else
        std::cerr << "MovablePlatform: EndAnchor '" << m_endAnchor << "' is not a character drawn in the level's picture" << std::endl;

    world.GetPhysics().Add(m_collider);
    return true;
}
