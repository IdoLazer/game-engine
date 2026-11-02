#include "Lever.h"
#include "../PlatformerWorld.h"

using namespace Engine;

namespace
{
    // The bar's length as a fraction of its region's height, and its thickness in world units.
    constexpr float kBarLength = 0.8f;
    constexpr float kBarThickness = 0.06f;

    // Which way the bar points: straight up, or leaning over to the right.
    constexpr Vec2 kUpright{0.0f, 1.0f};
    constexpr Vec2 kLeaning{0.6f, 0.8f};
}

// --- Type Registration ---

BEGIN_TYPE_REGISTER(Lever)
    REGISTER_PROPERTY(std::string, Switches, &Lever::m_switches)
END_TYPE_REGISTER()

// --- Lifecycle ---

void Lever::Render() const
{
    Vec2 base(m_worldPosition.x, m_worldPosition.y - m_worldSize.y * 0.5f);
    Vec2 direction = m_isOn ? kLeaning : kUpright;

    Renderer2D::DrawLine(base, base + direction * (m_worldSize.y * kBarLength), m_color, kBarThickness);
}

// --- Public Interface ---

bool Lever::Place(PlatformerWorld &world)
{
    if (!LevelRegion::Place(world))
        return false;

    m_collider.SetTrigger(true);
    world.GetPhysics().Add(m_collider);
    return true;
}

// --- IFalconTarget ---

FalconResponse Lever::OnFalconArrived()
{
    m_isOn = true;
    GetWorld().Switch(m_switches, true);
    return FalconResponse::Latch;
}

void Lever::OnFalconLeft()
{
    m_isOn = false;
    GetWorld().Switch(m_switches, false);
}
