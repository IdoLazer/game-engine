#pragma once

#include "FalconState.h"
#include "StateMachine.h"

// The falcon's request vocabulary, on top of the machine in StateMachine.h.
class FalconStateMachine : public StateMachine<FalconStateId, FalconState>
{
public:
    // --- Requests ---
    // Each returns whether a state honoured the request.
    bool AimRequested()      { return Dispatch([](FalconState &state) { return state.OnAimRequested(); }); }
    bool AimReleased()       { return Dispatch([](FalconState &state) { return state.OnAimReleased(); }); }
    bool RetrieveRequested() { return Dispatch([](FalconState &state) { return state.OnRetrieveRequested(); }); }
    bool GlideRequested()    { return Dispatch([](FalconState &state) { return state.OnGlideRequested(); }); }
    bool GlideReleased()     { return Dispatch([](FalconState &state) { return state.OnGlideReleased(); }); }

    // --- Rendering ---
    void Render() const
    {
        if (const FalconState *state = GetCurrentState()) state->Render();
    }

    // --- Accessors ---
    bool IsOnShoulder() const { return IsInState(FalconStateId::OnShoulder); }
};
