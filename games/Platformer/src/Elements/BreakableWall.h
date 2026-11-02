#pragma once

#include "../IFalconTarget.h"
#include "../Levels/LevelRegion.h"
#include "../Physics/Collider.h"
#include "../PlatformerLayers.h"

// A solid the falcon breaks by reaching it, and then flies back from.
class BreakableWall : public LevelRegion, public IFalconTarget
{
    DECLARE_TYPE(BreakableWall, LevelRegion)

// --- Lifecycle ---
public:
    void Render() const override;

// --- Public Interface ---
public:
    bool Place(PlatformerWorld &world) override;

// --- IFalconTarget ---
public:
    FalconResponse OnFalconArrived() override;
    void OnFalconLeft() override {}

// --- Private Fields ---
private:
    Collider m_collider{*this, Layer::Terrain | Layer::Interactive, 0};
};
