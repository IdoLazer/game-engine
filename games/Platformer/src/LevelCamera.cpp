#include "LevelCamera.h"

#include <algorithm>

using namespace Engine;

// --- Type Registration ---

BEGIN_TYPE_REGISTER(LevelCamera)
    REGISTER_PROPERTY(float, VisibleRows, &LevelCamera::m_visibleRows)
END_TYPE_REGISTER()

namespace
{
    // Keeps the view within the level, centering it on any axis where the level is smaller.
    Vec2 ViewCenter(const Vec2 &target, const Rect &levelBounds, const Vec2 &viewHalfSize)
    {
        Vec2 maxOffset(std::max(levelBounds.halfExtents.x - viewHalfSize.x, 0.0f),
                       std::max(levelBounds.halfExtents.y - viewHalfSize.y, 0.0f));
        Vec2 offset = target - levelBounds.center;

        return levelBounds.center + Vec2(std::clamp(offset.x, -maxOffset.x, maxOffset.x),
                                         std::clamp(offset.y, -maxOffset.y, maxOffset.y));
    }
}

// --- Lifecycle ---

void LevelCamera::Update(float deltaTime)
{
    if (!m_target)
        return;

    Camera2D &camera = Renderer2D::GetCamera();
    Vec2 viewHalfSize = Vec2(camera.GetWorldWidth(), camera.GetWorldHeight()) * 0.5f;
    camera.SetPosition(ViewCenter(m_target->GetWorldPosition(), m_levelBounds, viewHalfSize));
}

void LevelCamera::Destroy()
{
    Renderer2D::GetCamera().SetPosition(Vec2::Zero);
}

// --- Public Interface ---

void LevelCamera::Follow(const Entity *target, const Rect &levelBounds)
{
    m_target = target;
    m_levelBounds = levelBounds;
}
