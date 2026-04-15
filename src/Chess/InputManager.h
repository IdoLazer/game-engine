#pragma once

#include <Engine.h>

// InputManager handles converting raw keyboard input into Command objects
// and manages the command queue for buffered input
// This separates input detection from game logic execution

class InputManager
{
public:
    // --- Constructors & Destructors ---
    InputManager() = default;
    ~InputManager() = default;

    // --- Input Processing ---
    void ProcessInput();

    // --- Accessors ---
    bool IsExitRequested() const;

private:
    // --- Fields ---
    bool m_exitRequested{false};
};