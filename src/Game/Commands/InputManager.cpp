#include "InputManager.h"
#include "MovementCommands.h"

using namespace Engine;

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