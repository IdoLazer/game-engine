#include "Mouse.h"
#include "Rendering/Renderer2D.h"
#include <iostream>

namespace Engine
{
    // Static member definitions
    GLFWwindow *Mouse::m_Window = nullptr;
    std::unordered_map<int, bool> Mouse::m_CurrentButtonState;
    std::unordered_map<int, bool> Mouse::m_PreviousButtonState;
    Vec2 Mouse::m_CurrentPosition(0.0f, 0.0f);
    Vec2 Mouse::m_PreviousPosition(0.0f, 0.0f);
    Vec2 Mouse::m_ScrollOffset(0.0f, 0.0f);
    bool Mouse::m_Initialized = false;

    void Mouse::Initialize(GLFWwindow *window)
    {
        if (m_Initialized)
            return;

        m_Window = window;

        // Set up GLFW mouse callbacks
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetCursorPosCallback(window, CursorPositionCallback);
        glfwSetScrollCallback(window, ScrollCallback);

        // Initialize cursor position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        m_CurrentPosition = Vec2(static_cast<float>(xpos), static_cast<float>(ypos));
        m_PreviousPosition = m_CurrentPosition;

        m_Initialized = true;
        std::cout << "Mouse initialized with GLFW" << std::endl;
    }

    void Mouse::Shutdown()
    {
        if (!m_Initialized)
            return;

        glfwSetMouseButtonCallback(m_Window, nullptr);
        glfwSetCursorPosCallback(m_Window, nullptr);
        glfwSetScrollCallback(m_Window, nullptr);
        m_CurrentButtonState.clear();
        m_PreviousButtonState.clear();
        m_Window = nullptr;
        m_Initialized = false;
        std::cout << "Mouse shut down" << std::endl;
    }

    void Mouse::Update()
    {
        if (!m_Initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return;
        }

        m_PreviousButtonState = m_CurrentButtonState;
        m_PreviousPosition = m_CurrentPosition;

        // Reset scroll offset (scroll is per-frame)
        m_ScrollOffset = Vec2(0.0f, 0.0f);
    }

    void Mouse::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            m_CurrentButtonState[button] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_CurrentButtonState[button] = false;
        }
    }

    void Mouse::CursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
    {
        m_CurrentPosition = Vec2(static_cast<float>(xpos), static_cast<float>(ypos));
    }

    void Mouse::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        m_ScrollOffset = Vec2(static_cast<float>(xoffset), static_cast<float>(yoffset));
    }

    bool Mouse::IsButtonDown(int glfwButton)
    {
        if (!m_Initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        auto it = m_CurrentButtonState.find(glfwButton);
        return it != m_CurrentButtonState.end() && it->second;
    }

    bool Mouse::IsButtonPressed(int glfwButton)
    {
        if (!m_Initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        bool currentPressed = false;
        bool previousPressed = false;

        auto currentIt = m_CurrentButtonState.find(glfwButton);
        if (currentIt != m_CurrentButtonState.end())
            currentPressed = currentIt->second;

        auto previousIt = m_PreviousButtonState.find(glfwButton);
        if (previousIt != m_PreviousButtonState.end())
            previousPressed = previousIt->second;

        return currentPressed && !previousPressed;
    }

    bool Mouse::IsButtonReleased(int glfwButton)
    {
        if (!m_Initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return false;
        }

        bool currentPressed = false;
        bool previousPressed = false;

        auto currentIt = m_CurrentButtonState.find(glfwButton);
        if (currentIt != m_CurrentButtonState.end())
            currentPressed = currentIt->second;

        auto previousIt = m_PreviousButtonState.find(glfwButton);
        if (previousIt != m_PreviousButtonState.end())
            previousPressed = previousIt->second;

        return !currentPressed && previousPressed;
    }

    Vec2 Mouse::GetPosition()
    {
        if (!m_Initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return Vec2(0.0f, 0.0f);
        }

        return m_CurrentPosition;
    }

    Vec2 Mouse::GetWorldPosition()
    {
        if (!m_Initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return Vec2(0.0f, 0.0f);
        }

        return Renderer2D::GetCamera().ScreenToWorld(m_CurrentPosition);
    }

    Vec2 Mouse::GetDelta()
    {
        if (!m_Initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return Vec2(0.0f, 0.0f);
        }

        return m_CurrentPosition - m_PreviousPosition;
    }

    Vec2 Mouse::GetScrollOffset()
    {
        if (!m_Initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return Vec2(0.0f, 0.0f);
        }

        return m_ScrollOffset;
    }

    void Mouse::SetCursorVisibility(bool visible)
    {
        if (!m_Initialized)
        {
            std::cerr << "Mouse not initialized! Call Initialize() first." << std::endl;
            return;
        }

        glfwSetInputMode(m_Window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }
}