#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include "../Math/Vec2.h"
#include "MouseCodes.h"
#include "Events/Event.h"

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
        static Vec2 GetScreenPosition();
        static Vec2 GetWorldPosition();
        static Vec2 GetScreenDelta();

        // --- Scroll Queries ---
        static Vec2 GetScrollOffset();

        // --- Cursor ---
        static void SetCursorVisibility(bool visible);

        // --- Events ---
        static EventSubscriber<void(MouseButton button)> &OnButtonPressed() { return s_buttonPressed; }
        static EventSubscriber<void(MouseButton button)> &OnButtonReleased() { return s_buttonReleased; }
        static EventSubscriber<void(Vec2 screenPosition)> &OnMoved() { return s_moved; }
        static EventSubscriber<void(Vec2 scrollOffset)> &OnScrolled() { return s_scrolled; }

    private:
        // --- Fields ---
        static GLFWwindow *m_window;
        static std::unordered_map<int, bool> m_currentButtonState;
        static std::unordered_map<int, bool> m_previousButtonState;
        static Vec2 m_currentScreenPosition;
        static Vec2 m_previousScreenPosition;
        static Vec2 m_scrollOffset;
        static bool m_initialized;

        // --- Internal ---
        static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

        // --- Events ---
        static Event<void(MouseButton button)> s_buttonPressed;
        static Event<void(MouseButton button)> s_buttonReleased;
        static Event<void(Vec2 screenPosition)> s_moved;
        static Event<void(Vec2 scrollOffset)> s_scrolled;
    };
}