#pragma once

#include "../Levels/LevelRegion.h"
#include "../Physics/Collider.h"
#include "../PlatformerLayers.h"

// A coin the player collects by touching it. The session remembers it, so it doesn't come back.
class Coin : public LevelRegion
{
    DECLARE_TYPE(Coin, LevelRegion)

// --- Lifecycle ---
public:
    void Render() const override;

// --- Public Interface ---
public:
    // A coin the session already holds goes at once.
    bool Place(PlatformerWorld &world) override;

// --- Internal ---
private:
    void OnEntered(const Collider &other);

// --- Private Fields ---
private:
    Collider m_collider{*this, 0, Layer::Player};
    Engine::Subscription m_enteredSubscription; // After m_collider, so it unsubscribes while the event still exists.
};
