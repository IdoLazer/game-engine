#include "LevelCamera.h"

#include <algorithm>

using namespace Engine;

// --- Type Registration ---

BEGIN_TYPE_REGISTER(LevelCamera)
    REGISTER_PROPERTY(float, VisibleRows, &LevelCamera::m_visibleRows)
END_TYPE_REGISTER()

namespace
{
    // The view's center along one axis: on the target, stopped at the level's edges, or on the
    // level's middle when the level is smaller than the view.
    float ViewCenter(float target, float levelMin, float levelMax, float viewHalfSize)
    {
        if (levelMax - levelMin <= viewHalfSize * 2.0f)
            return (levelMin + levelMax) * 0.5f;

        return std::clamp(target, levelMin + viewHalfSize, levelMax - viewHalfSize);
    }
}

// --- Lifecycle ---

void LevelCamera::Update(float deltaTime)
{
    if (!m_target)
        return;

    Camera2D &camera = Renderer2D::GetCamera();
    Vec2 target = m_target->GetWorldPosition();
    Vec2 levelMin = m_levelBounds.Min();
    Vec2 levelMax = m_levelBounds.Max();

    camera.SetPosition(Vec2(
        ViewCenter(target.x, levelMin.x, levelMax.x, camera.GetWorldWidth() * 0.5f),
        ViewCenter(target.y, levelMin.y, levelMax.y, camera.GetWorldHeight() * 0.5f)));
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
