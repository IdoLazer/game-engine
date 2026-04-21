#include "MovementInputHandler.h"
#include "MovementCommands.h"

using namespace Engine;

// --- Constructors & Destructors ---

MovementInputHandler::MovementInputHandler(IMovable *movable) : m_movable(movable)
{
    m_movementCommandQueue.SetConflictResolver(std::make_unique<MovementCommandConflictResolver>());

    m_keySub = Keyboard::OnKeyPressed().Subscribe([this](const Key &key)
    {
        switch (key)
        {
            case Key::Up:    m_movementCommandQueue.EnqueueCommand(std::make_unique<MoveUpCommand>(m_movable)); break;
            case Key::Down:  m_movementCommandQueue.EnqueueCommand(std::make_unique<MoveDownCommand>(m_movable)); break;
            case Key::Left:  m_movementCommandQueue.EnqueueCommand(std::make_unique<MoveLeftCommand>(m_movable)); break;
            case Key::Right: m_movementCommandQueue.EnqueueCommand(std::make_unique<MoveRightCommand>(m_movable)); break;
            default: break;
        }
    });
}

std::unique_ptr<Command> MovementInputHandler::GetNextMovementCommand()
{
    return m_movementCommandQueue.DequeueCommand();
}

// --- Accessors ---

bool MovementInputHandler::HasQueuedMovementCommands() const { return m_movementCommandQueue.HasCommands(); }
size_t MovementInputHandler::GetMovementQueueSize() const { return m_movementCommandQueue.GetSize(); }

// --- Control ---

void MovementInputHandler::ClearMovementCommands() { m_movementCommandQueue.Clear(); }