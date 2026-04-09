#include "InputManager.h"

using namespace Engine;

void InputManager::ProcessInput()
{
    // Check for keyboard input and update the exitRequested flag
    if (Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        m_exitRequested = true;
    }
}
