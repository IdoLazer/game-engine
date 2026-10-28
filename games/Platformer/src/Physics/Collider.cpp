#include "Collider.h"
#include "Physics.h"

using namespace Engine;

// --- Constructors & Destructors ---

Collider::Collider(GridEntity &owner, LayerMask layer, LayerMask mask)
    : m_owner(owner), m_layer(layer), m_mask(mask)
{
}

Collider::~Collider()
{
    if (m_physics)
        m_physics->Remove(*this);
}

// --- Accessors ---

Rect Collider::GetRect() const
{
    return Rect(m_owner.GetGridPosition(), m_owner.GetGridSize() * 0.5f);
}

// --- Movement & Contact ---

MoveResult Collider::MoveAndSlide(const Vec2 &delta)
{
    if (!m_physics)
    {
        m_owner.SetGridPosition(m_owner.GetGridPosition() + delta);
        return {};
    }

    return m_physics->MoveAndSlide(*this, delta);
}

void Collider::MoveAndPush(const Vec2 &delta)
{
    if (!m_physics)
    {
        m_owner.SetGridPosition(m_owner.GetGridPosition() + delta);
        return;
    }

    m_physics->MoveAndPush(*this, delta);
}

bool Collider::TouchesSolid(const Vec2 &direction) const
{
    return m_physics && m_physics->TouchesSolid(GetRect(), direction, m_mask, this);
}
