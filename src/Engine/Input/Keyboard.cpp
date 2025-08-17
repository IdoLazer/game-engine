#include "Keyboard.h"
#include <iostream>

namespace Engine
{
    // Static member definitions
    GLFWwindow *Keyboard::m_Window = nullptr;
    std::unordered_map<int, bool> Keyboard::m_CurrentKeyState;
    std::unordered_map<int, bool> Keyboard::m_PreviousKeyState;
    bool Keyboard::m_Initialized = false;

    void Keyboard::Initialize(GLFWwindow *window)
    {
        if (m_Initialized)
            return;

        m_Window = window;

        // Set up GLFW key callback
        glfwSetKeyCallback(window, KeyCallback);

        m_Initialized = true;
        std::cout << "Keyboard initialized with GLFW" << std::endl;
    }

    void Keyboard::Update()
    {
        if (!m_Initialized)
        {
            std::cerr << "Keyboard not initialized! Call Initialize() first." << std::endl;
            return;
        }

        m_PreviousKeyState = m_CurrentKeyState;
    }

    void Keyboard::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            m_CurrentKeyState[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_CurrentKeyState[key] = false;
        }
    }

    bool Keyboard::IsKeyDown(int glfwKey)
    {
        if (!m_Initialized)
        {
            std::cerr << "Keyboard not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        auto it = m_CurrentKeyState.find(glfwKey);
        return it != m_CurrentKeyState.end() && it->second;
    }

    bool Keyboard::IsKeyPressed(int glfwKey)
    {
        if (!m_Initialized)
        {
            std::cerr << "Keyboard not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        bool currentPressed = false;
        bool previousPressed = false;

        auto currentIt = m_CurrentKeyState.find(glfwKey);
        if (currentIt != m_CurrentKeyState.end())
            currentPressed = currentIt->second;

        auto previousIt = m_PreviousKeyState.find(glfwKey);
        if (previousIt != m_PreviousKeyState.end())
            previousPressed = previousIt->second;

        return currentPressed && !previousPressed;
    }

    bool Keyboard::IsKeyReleased(int glfwKey)
    {
        if (!m_Initialized)
        {
            std::cerr << "Keyboard not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        bool currentPressed = false;
        bool previousPressed = false;

        auto currentIt = m_CurrentKeyState.find(glfwKey);
        if (currentIt != m_CurrentKeyState.end())
            currentPressed = currentIt->second;

        auto previousIt = m_PreviousKeyState.find(glfwKey);
        if (previousIt != m_PreviousKeyState.end())
            previousPressed = previousIt->second;

        return !currentPressed && previousPressed;
    }
}