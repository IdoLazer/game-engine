#include "CursorEntity.h"

using namespace Engine;

// --- Type Registration ---

BEGIN_TYPE_REGISTER(CursorEntity)
END_TYPE_REGISTER()

// --- Lifecycle ---

void CursorEntity::Update(float deltaTime)
{
    m_worldPosition = Mouse::GetWorldPosition();
}

void CursorEntity::Render() const
{
    Renderer2D::DrawCircle(m_worldPosition, m_radius, m_color);
}

// --- Accessors ---

void CursorEntity::SetRadius(float radius) { m_radius = radius; }
