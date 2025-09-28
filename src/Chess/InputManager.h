#pragma once

#include <Engine.h>

using namespace Engine;

// InputManager handles converting raw keyboard input into Command objects
// and manages the command queue for buffered input
// This separates input detection from game logic execution

class InputManager
{
private:
    bool m_exitRequested{false};

public:
    InputManager() = default;

    ~InputManager() = default;

    // Process keyboard input and convert to commands
    void ProcessInput();

    bool IsExitRequested() const { return m_exitRequested; }
};