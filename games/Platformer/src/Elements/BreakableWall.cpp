#include "BreakableWall.h"
#include "../PlatformerWorld.h"

using namespace Engine;

// --- Type Registration ---

BEGIN_TYPE_REGISTER(BreakableWall)
END_TYPE_REGISTER()

// --- Lifecycle ---

void BreakableWall::Render() const
{
    Renderer2D::DrawTile(m_worldPosition, m_worldSize, m_color);
}

// --- Public Interface ---

bool BreakableWall::Place(PlatformerWorld &world)
{
    if (!LevelRegion::Place(world))
        return false;

    world.GetPhysics().Add(m_collider);
    return true;
}

// --- IFalconTarget ---

FalconResponse BreakableWall::OnFalconArrived()
{
    GetScene()->Destroy(this);
    return FalconResponse::Return;
}
