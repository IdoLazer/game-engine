#pragma once

#include "../IFalconTarget.h"
#include "../Levels/LevelRegion.h"
#include "../Physics/Collider.h"
#include "../PlatformerLayers.h"

#include <string>

// A trigger the falcon holds on to: it switches the regions named by Switches on while the falcon is latched to it.
class Lever : public LevelRegion, public IFalconTarget
{
    DECLARE_TYPE(Lever, LevelRegion)

// --- Lifecycle ---
public:
    void Render() const override;

// --- Public Interface ---
public:
    bool Place(PlatformerWorld &world) override;

// --- IFalconTarget ---
public:
    FalconResponse OnFalconArrived() override;
    void OnFalconLeft() override;

// --- Configuration (data-driven via type registry) ---
private:
    std::string m_switches;

// --- Private Fields ---
private:
    bool m_isOn{false};
    Collider m_collider{*this, Layer::Interactive, 0};
};
