#pragma once

#include "MovementCommands.h"
#include <Engine.h>

class MovementInputHandler
{
public:
    // --- Constructors & Destructors ---
    MovementInputHandler() = delete;
    MovementInputHandler(IMovable *movable);
    ~MovementInputHandler() = default;

    // --- Accessors ---
    std::unique_ptr<Engine::Command> GetNextMovementCommand();
    bool HasQueuedMovementCommands() const;
    size_t GetMovementQueueSize() const;

    // --- Control ---
    void ClearMovementCommands();

private:
    // --- Fields ---
    Engine::CommandQueue m_movementCommandQueue;
    IMovable *m_movable{nullptr};
    Engine::Subscription m_keySub;
};