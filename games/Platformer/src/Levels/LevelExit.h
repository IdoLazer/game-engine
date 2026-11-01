#pragma once

#include "LevelRegion.h"
#include "../Physics/Collider.h"
#include "../PlatformerLayers.h"

#include <string>

// A region of a level that leads to a spawn point in another level when the player enters it.
class LevelExit : public LevelRegion
{
    DECLARE_TYPE(LevelExit, LevelRegion)

// --- Public Interface ---
public:
    bool Place(PlatformerWorld &world) override;

// --- Internal ---
private:
    void OnEntered(const Collider &other);

// --- Configuration (data-driven via type registry) ---
private:
    std::string m_targetLevel;
    std::string m_targetSpawn;

// --- Private Fields ---
private:
    Collider m_collider{*this, 0, Layer::Player};
    Engine::Subscription m_enteredSubscription; // After m_collider, so it unsubscribes while the event still exists.
};
