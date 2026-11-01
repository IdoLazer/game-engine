#pragma once

#include "../Levels/LevelRegion.h"
#include "../Physics/Collider.h"
#include "../PlatformerLayers.h"

#include <string>

// A trigger that switches the regions named by Switches on while the player is on it.
class PressurePlate : public LevelRegion
{
    DECLARE_TYPE(PressurePlate, LevelRegion)

// --- Lifecycle ---
public:
    void Render() const override;

// --- Public Interface ---
public:
    bool Place(PlatformerWorld &world) override;

// --- Internal ---
private:
    void OnEntered(const Collider &other);
    void OnExited(const Collider &other);

// --- Configuration (data-driven via type registry) ---
private:
    std::string m_switches;

// --- Private Fields ---
private:
    int m_occupantCount{0};
    Collider m_collider{*this, 0, Layer::Player};
    // After m_collider, so they unsubscribe while its events still exist.
    Engine::Subscription m_enteredSubscription;
    Engine::Subscription m_exitedSubscription;
};
