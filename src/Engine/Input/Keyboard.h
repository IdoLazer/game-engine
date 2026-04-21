#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include "KeyCodes.h"
#include "Events/Event.h"

namespace Engine
{
    class Keyboard
    {
    public:
        // --- Lifecycle ---
        static void Initialize(GLFWwindow *window);
        static void Shutdown();
        static void Update();

        // --- Queries ---
        static bool IsKeyDown(Key key);
        static bool IsKeyPressed(Key key);
        static bool IsKeyReleased(Key key);

        // --- Events ---
        static EventSubscriber<Key> &OnKeyPressed() { return s_keyPressed; }
        static EventSubscriber<Key> &OnKeyReleased() { return s_keyReleased; }

    private:
        // --- Fields ---
        static GLFWwindow *m_window;
        static std::unordered_map<int, bool> m_currentKeyState;
        static std::unordered_map<int, bool> m_previousKeyState;
        static bool m_initialized;

        // --- Internal ---
        static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

        // --- Events ---
        static Event<Key> s_keyPressed;
        static Event<Key> s_keyReleased;
    };
}