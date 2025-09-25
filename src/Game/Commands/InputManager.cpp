#include "InputManager.h"
#include "MovementCommands.h"

void InputManager::ProcessInput()
{
    m_exitRequested = Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE);

    bool isUpPressed = Keyboard::IsKeyPressed(GLFW_KEY_UP);
    bool isDownPressed = Keyboard::IsKeyPressed(GLFW_KEY_DOWN);
    bool isLeftPressed = Keyboard::IsKeyPressed(GLFW_KEY_LEFT);
    bool isRightPressed = Keyboard::IsKeyPressed(GLFW_KEY_RIGHT);

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