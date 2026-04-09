#include "Keyboard.h"
#include <iostream>

namespace Engine
{
    // Static member definitions
    GLFWwindow *Keyboard::m_window = nullptr;
    std::unordered_map<int, bool> Keyboard::m_currentKeyState;
    std::unordered_map<int, bool> Keyboard::m_previousKeyState;
    bool Keyboard::m_initialized = false;

    void Keyboard::Initialize(GLFWwindow *window)
    {
        if (m_initialized)
            return;

        m_window = window;

        // Set up GLFW key callback
        glfwSetKeyCallback(window, KeyCallback);

        m_initialized = true;
        std::cout << "Keyboard initialized with GLFW" << std::endl;
    }

    void Keyboard::Shutdown()
    {
        if (!m_initialized)
            return;

        glfwSetKeyCallback(m_window, nullptr);
        m_currentKeyState.clear();
        m_previousKeyState.clear();
        m_window = nullptr;
        m_initialized = false;
        std::cout << "Keyboard shut down" << std::endl;
    }

    void Keyboard::Update()
    {
        if (!m_initialized)
        {
            std::cerr << "Keyboard not initialized! Call Initialize() first." << std::endl;
            return;
        }

        m_previousKeyState = m_currentKeyState;
    }

    void Keyboard::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            m_currentKeyState[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_currentKeyState[key] = false;
        }
    }

    bool Keyboard::IsKeyDown(int glfwKey)
    {
        if (!m_initialized)
        {
            std::cerr << "Keyboard not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        auto it = m_currentKeyState.find(glfwKey);
        return it != m_currentKeyState.end() && it->second;
    }

    bool Keyboard::IsKeyPressed(int glfwKey)
    {
        if (!m_initialized)
        {
            std::cerr << "Keyboard not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        bool currentPressed = false;
        bool previousPressed = false;

        auto currentIt = m_currentKeyState.find(glfwKey);
        if (currentIt != m_currentKeyState.end())
            currentPressed = currentIt->second;

        auto previousIt = m_previousKeyState.find(glfwKey);
        if (previousIt != m_previousKeyState.end())
            previousPressed = previousIt->second;

        return currentPressed && !previousPressed;
    }

    bool Keyboard::IsKeyReleased(int glfwKey)
    {
        if (!m_initialized)
        {
            std::cerr << "Keyboard not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        bool currentPressed = false;
        bool previousPressed = false;

        auto currentIt = m_currentKeyState.find(glfwKey);
        if (currentIt != m_currentKeyState.end())
            currentPressed = currentIt->second;

        auto previousIt = m_previousKeyState.find(glfwKey);
        if (previousIt != m_previousKeyState.end())
            previousPressed = previousIt->second;

        return !currentPressed && previousPressed;
    }
}