#pragma once

#include "MovementCommands.h"
#include <Engine.h>

// InputManager handles converting raw keyboard input into Command objects
// and manages the command queue for buffered input
// This separates input detection from game logic execution

class InputManager
{
public:
    // --- Constructors & Destructors ---
    InputManager() = delete;
    InputManager(IMovable *movable);
    ~InputManager() = default;

    // --- Input Processing ---
    void ProcessInput();
    std::unique_ptr<Engine::Command> GetNextMovementCommand();

    // --- Accessors ---
    bool HasQueuedMovementCommands() const;
    size_t GetMovementQueueSize() const;
    bool IsExitRequested() const;

    // --- Control ---
    void ClearMovementCommands();

private:
    // --- Fields ---
    Engine::CommandQueue m_movementCommandQueue;
    class IMovable *m_movable{nullptr};
    bool m_exitRequested{false};
};