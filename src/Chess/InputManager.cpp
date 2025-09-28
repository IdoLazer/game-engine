#include "InputManager.h"

void InputManager::ProcessInput()
{
    // Check for keyboard input and update the exitRequested flag
    if (Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        m_exitRequested = true;
    }
}
