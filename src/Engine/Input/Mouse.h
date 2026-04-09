#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include "../Math/Vec2.h"

namespace Engine
{
    class Mouse
    {
    public:
        static void Initialize(GLFWwindow *window);
        static void Shutdown();
        static void Update();

        // Mouse button queries
        static bool IsButtonDown(int glfwButton);
        static bool IsButtonPressed(int glfwButton);
        static bool IsButtonReleased(int glfwButton);

        // Mouse position queries
        static Vec2 GetPosition();
        static Vec2 GetWorldPosition();
        static Vec2 GetDelta();

        // Mouse scroll queries
        static Vec2 GetScrollOffset();

        static void SetCursorVisibility(bool visible);

    private:
        static GLFWwindow *m_window;
        static std::unordered_map<int, bool> m_currentButtonState;
        static std::unordered_map<int, bool> m_previousButtonState;
        static Vec2 m_currentPosition;
        static Vec2 m_previousPosition;
        static Vec2 m_scrollOffset;
        static bool m_initialized;

        static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    };
}