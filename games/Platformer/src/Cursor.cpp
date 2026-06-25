#include "Cursor.h"

using namespace Engine;

// --- Type Registration ---

BEGIN_TYPE_REGISTER(Cursor)
    REGISTER_PROPERTY(float, Radius, &Cursor::m_radius)
END_TYPE_REGISTER()

// --- Lifecycle ---

void Cursor::Render() const
{
    Renderer2D::DrawCircle(m_worldPosition, m_radius, m_color);
}
