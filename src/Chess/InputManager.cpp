#include "InputManager.h"

using namespace Engine;

// --- Input Processing ---

void InputManager::ProcessInput()
{
    if (Keyboard::IsKeyPressed(Key::Escape))
    {
        m_exitRequested = true;
    }
}

// --- Accessors ---

bool InputManager::IsExitRequested() const { return m_exitRequested; }
