#include "InputManager.h"
#include "MovementCommands.h"

using namespace Engine;

// --- Constructors & Destructors ---

InputManager::InputManager(IMovable *movable) : m_movable(movable)
{
    m_movementCommandQueue.SetConflictResolver(std::make_unique<MovementCommandConflictResolver>());
}

// --- Input Processing ---

void InputManager::ProcessInput()
{
    m_exitRequested = Keyboard::IsKeyPressed(Key::Escape);

    bool isUpPressed = Keyboard::IsKeyPressed(Key::Up);
    bool isDownPressed = Keyboard::IsKeyPressed(Key::Down);
    bool isLeftPressed = Keyboard::IsKeyPressed(Key::Left);
    bool isRightPressed = Keyboard::IsKeyPressed(Key::Right);

    if (isUpPressed)
    {
        m_movementCommandQueue.EnqueueCommand(std::make_unique<MoveUpCommand>(m_movable));
    }
    if (isDownPressed)
    {
        m_movementCommandQueue.EnqueueCommand(std::make_unique<MoveDownCommand>(m_movable));
    }
    if (isLeftPressed)
    {
        m_movementCommandQueue.EnqueueCommand(std::make_unique<MoveLeftCommand>(m_movable));
    }
    if (isRightPressed)
    {
        m_movementCommandQueue.EnqueueCommand(std::make_unique<MoveRightCommand>(m_movable));
    }
}

std::unique_ptr<Command> InputManager::GetNextMovementCommand()
{
    return m_movementCommandQueue.DequeueCommand();
}

// --- Accessors ---

bool InputManager::HasQueuedMovementCommands() const { return m_movementCommandQueue.HasCommands(); }
size_t InputManager::GetMovementQueueSize() const { return m_movementCommandQueue.GetSize(); }
bool InputManager::IsExitRequested() const { return m_exitRequested; }

// --- Control ---

void InputManager::ClearMovementCommands() { m_movementCommandQueue.Clear(); }