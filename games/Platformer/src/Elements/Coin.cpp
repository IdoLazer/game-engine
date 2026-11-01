#include "Coin.h"
#include "../PlatformerWorld.h"
#include "../Session.h"

using namespace Engine;

namespace
{
    constexpr Color kYellow{1.0f, 1.0f, 0.0f, 1.0f};
}

// --- Type Registration ---

BEGIN_TYPE_REGISTER(Coin)
END_TYPE_REGISTER()

// --- Lifecycle ---

void Coin::Render() const
{
    Renderer2D::DrawCircle(GetWorldPosition(), 0.1f, kYellow);
}

// --- Public Interface ---

bool Coin::Place(PlatformerWorld &world)
{
    if (!LevelRegion::Place(world))
        return false;

    if (world.GetSession().IsCollected(GetAnchor()))
    {
        GetScene()->Destroy(this);
        return true;
    }

    m_collider.SetTrigger(true);
    world.GetPhysics().Add(m_collider);
    m_enteredSubscription = m_collider.OnEntered().Subscribe(this, &Coin::OnEntered);
    return true;
}

// --- Internal ---

void Coin::OnEntered(const Collider &other)
{
    GetWorld().GetSession().Collect(GetAnchor());
    GetScene()->Destroy(this);
}
