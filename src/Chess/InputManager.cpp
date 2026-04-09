#include "InputManager.h"

using namespace Engine;

void InputManager::ProcessInput()
{
    // Check for keyboard input and update the exitRequested flag
    if (Keyboard::IsKeyPressed(Key::Escape))
    {
        m_exitRequested = true;
    }
}
