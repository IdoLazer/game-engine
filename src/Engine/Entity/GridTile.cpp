#include "GridTile.h"
#include "../Rendering/Renderer2D.h"

namespace Engine
{

// --- Type Registration ---

BEGIN_TYPE_REGISTER(GridTile)
END_TYPE_REGISTER()

// --- Lifecycle ---

void GridTile::Render() const
{
    Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);
}

}
