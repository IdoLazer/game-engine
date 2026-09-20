#pragma once

#include "State.h"
#include <Engine.h>

// --- Forward Declarations ---
class Falcon;
class FalconStateMachine;

// --- Types ---
enum class FalconStateId
{
    OnShoulder, // Resting on the player's shoulder, following them
    Aiming,     // While on the shoulder, aiming at the cursor but not yet released
    Flying,     // Moving out towards a latch point after being released
    Returning,  // Flying back to the player after Retrieve()
    Latched,    // Stuck into a surface, acting as a fixed hinge point
    Gliding     // Gliding with the player, reducing their fall speed
};

// One mode of the falcon's behavior, adding its request vocabulary on top of
// the generic lifecycle in State.h.
class FalconState : public State<FalconStateId>
{
public:
    FalconState(Falcon &falcon, FalconStateMachine &machine)
        : m_falcon(falcon), m_machine(machine) {}

    // --- Requests ---
    // Return true to consume the request.
    virtual bool OnAimRequested() { return false; }
    virtual bool OnAimReleased() { return false; }
    virtual bool OnRetrieveRequested() { return false; }
    virtual bool OnGlideRequested() { return false; }
    virtual bool OnGlideReleased() { return false; }

    // --- Rendering ---
    virtual void Render() const;

protected:
    // --- Fields ---
    Falcon &m_falcon;
    FalconStateMachine &m_machine;
};
