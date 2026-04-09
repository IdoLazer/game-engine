#include "Mouse.h"
#include "Rendering/Renderer2D.h"
#include <iostream>

namespace Engine
{
    // Static member definitions
    GLFWwindow *Mouse::m_window = nullptr;
    std::unordered_map<int, bool> Mouse::m_currentButtonState;
    std::unordered_map<int, bool> Mouse::m_previousButtonState;
    Vec2 Mouse::m_currentPosition(0.0f, 0.0f);
    Vec2 Mouse::m_previousPosition(0.0f, 0.0f);
    Vec2 Mouse::m_scrollOffset(0.0f, 0.0f);
    bool Mouse::m_initialized = false;

    void Mouse::Initialize(GLFWwindow *window)
    {
        if (m_initialized)
            return;

        m_window = window;

        // Set up GLFW mouse callbacks
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetCursorPosCallback(window, CursorPositionCallback);
        glfwSetScrollCallback(window, ScrollCallback);

        // Initialize cursor position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        m_currentPosition = Vec2(static_cast<float>(xpos), static_cast<float>(ypos));
        m_previousPosition = m_currentPosition;

        m_initialized = true;
        std::cout << "Mouse initialized with GLFW" << std::endl;
    }

    void Mouse::Shutdown()
    {
        if (!m_initialized)
            return;

        glfwSetMouseButtonCallback(m_window, nullptr);
        glfwSetCursorPosCallback(m_window, nullptr);
        glfwSetScrollCallback(m_window, nullptr);
        m_currentButtonState.clear();
        m_previousButtonState.clear();
        m_window = nullptr;
        m_initialized = false;
        std::cout << "Mouse shut down" << std::endl;
    }

    void Mouse::Update()
    {
        if (!m_initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return;
        }

        m_previousButtonState = m_currentButtonState;
        m_previousPosition = m_currentPosition;

        // Reset scroll offset (scroll is per-frame)
        m_scrollOffset = Vec2(0.0f, 0.0f);
    }

    void Mouse::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            m_currentButtonState[button] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_currentButtonState[button] = false;
        }
    }

    void Mouse::CursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
    {
        m_currentPosition = Vec2(static_cast<float>(xpos), static_cast<float>(ypos));
    }

    void Mouse::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        m_scrollOffset = Vec2(static_cast<float>(xoffset), static_cast<float>(yoffset));
    }

    bool Mouse::IsButtonDown(int glfwButton)
    {
        if (!m_initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        auto it = m_currentButtonState.find(glfwButton);
        return it != m_currentButtonState.end() && it->second;
    }

    bool Mouse::IsButtonPressed(int glfwButton)
    {
        if (!m_initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        bool currentPressed = false;
        bool previousPressed = false;

        auto currentIt = m_currentButtonState.find(glfwButton);
        if (currentIt != m_currentButtonState.end())
            currentPressed = currentIt->second;

        auto previousIt = m_previousButtonState.find(glfwButton);
        if (previousIt != m_previousButtonState.end())
            previousPressed = previousIt->second;

        return currentPressed && !previousPressed;
    }

    bool Mouse::IsButtonReleased(int glfwButton)
    {
        if (!m_initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        bool currentPressed = false;
        bool previousPressed = false;

        auto currentIt = m_currentButtonState.find(glfwButton);
        if (currentIt != m_currentButtonState.end())
            currentPressed = currentIt->second;

        auto previousIt = m_previousButtonState.find(glfwButton);
        if (previousIt != m_previousButtonState.end())
            previousPressed = previousIt->second;

        return !currentPressed && previousPressed;
    }

    Vec2 Mouse::GetPosition()
    {
        if (!m_initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return Vec2(0.0f, 0.0f);
        }

        return m_currentPosition;
    }

    Vec2 Mouse::GetWorldPosition()
    {
        if (!m_initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return Vec2(0.0f, 0.0f);
        }

        return Renderer2D::GetCamera().ScreenToWorld(m_currentPosition);
    }

    Vec2 Mouse::GetDelta()
    {
        if (!m_initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return Vec2(0.0f, 0.0f);
        }

        return m_currentPosition - m_previousPosition;
    }

    Vec2 Mouse::GetScrollOffset()
    {
        if (!m_initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return Vec2(0.0f, 0.0f);
        }

        return m_scrollOffset;
    }

    void Mouse::SetCursorVisibility(bool visible)
    {
        if (!m_initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return;
        }

        glfwSetInputMode(m_window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
}