#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include "../Math/Vec2.h"
#include "MouseCodes.h"

namespace Engine
{
    class Mouse
    {
    public:
        // --- Lifecycle ---
        static void Initialize(GLFWwindow *window);
        static void Shutdown();
        static void Update();

        // --- Button Queries ---
        static bool IsButtonDown(MouseButton button);
        static bool IsButtonPressed(MouseButton button);
        static bool IsButtonReleased(MouseButton button);

        // --- Position Queries ---
        static Vec2 GetPosition();
        static Vec2 GetWorldPosition();
        static Vec2 GetDelta();

        // --- Scroll Queries ---
        static Vec2 GetScrollOffset();

        // --- Cursor ---
        static void SetCursorVisibility(bool visible);

    private:
        // --- Fields ---
        static GLFWwindow *m_window;
        static std::unordered_map<int, bool> m_currentButtonState;
        static std::unordered_map<int, bool> m_previousButtonState;
        static Vec2 m_currentPosition;
        static Vec2 m_previousPosition;
        static Vec2 m_scrollOffset;
        static bool m_initialized;

        // --- Internal ---
        static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    };
}