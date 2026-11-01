#include "PressurePlate.h"
#include "../PlatformerWorld.h"

using namespace Engine;

namespace
{
    // The plate's thickness, as a fraction of its region's height.
    constexpr float kRaisedThickness = 0.3f;
    constexpr float kPressedThickness = 0.1f;
}

// --- Type Registration ---

BEGIN_TYPE_REGISTER(PressurePlate)
    REGISTER_PROPERTY(std::string, Switches, &PressurePlate::m_switches)
END_TYPE_REGISTER()

// --- Lifecycle ---

void PressurePlate::Render() const
{
    bool isPressed = m_occupantCount > 0;
    float thickness = m_worldSize.y * (isPressed ? kPressedThickness : kRaisedThickness);
    float bottom = m_worldPosition.y - m_worldSize.y * 0.5f;

    Renderer2D::DrawTile(Vec2(m_worldPosition.x, bottom + thickness * 0.5f), Vec2(m_worldSize.x, thickness), m_color);
}

// --- Public Interface ---

bool PressurePlate::Place(PlatformerWorld &world)
{
    if (!LevelRegion::Place(world))
        return false;

    m_collider.SetTrigger(true);
    world.GetPhysics().Add(m_collider);
    m_enteredSubscription = m_collider.OnEntered().Subscribe(this, &PressurePlate::OnEntered);
    m_exitedSubscription = m_collider.OnExited().Subscribe(this, &PressurePlate::OnExited);
    return true;
}

// --- Internal ---

void PressurePlate::OnEntered(const Collider &other)
{
    ++m_occupantCount;
    if (m_occupantCount == 1)
        GetWorld().Switch(m_switches, true);
}

void PressurePlate::OnExited(const Collider &other)
{
    --m_occupantCount;
    if (m_occupantCount == 0)
        GetWorld().Switch(m_switches, false);
}
