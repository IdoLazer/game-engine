#pragma once

#include "MovementCommands.h"
#include <Engine.h>

// InputManager handles converting raw keyboard input into Command objects
// and manages the command queue for buffered input
// This separates input detection from game logic execution

class InputManager
{
private:
    Engine::CommandQueue m_movementCommandQueue;

    // Reference to the player to create commands for
    class IMovable *m_movable{nullptr};

    bool m_exitRequested{false};

public:
    InputManager() = delete;

    InputManager(IMovable *movable) : m_movable(movable)
    {
        m_movementCommandQueue.SetConflictResolver(std::make_unique<MovementCommandConflictResolver>());
    }
    ~InputManager() = default;

    // Process keyboard input and convert to commands
    void ProcessInput();

    // Get the next command for execution (used by Player)
    std::unique_ptr<Engine::Command> GetNextMovementCommand();

    // Check if there are queued commands
    bool HasQueuedMovementCommands() const
    {
        return m_movementCommandQueue.HasCommands();
    }

    // Clear all queued commands (useful for game resets)
    void ClearMovementCommands() { m_movementCommandQueue.Clear(); }

    // Get queue size for debugging
    size_t GetMovementQueueSize() const { return m_movementCommandQueue.GetSize(); }

    bool IsExitRequested() const { return m_exitRequested; }
};