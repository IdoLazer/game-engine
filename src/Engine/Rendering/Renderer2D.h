#pragma once

#include <GLFW/glfw3.h>
#include "Window.h"
#include "Camera2D.h"
#include "Graphics/Color.h"
#include "Math/Vec2.h"

namespace Engine
{
    class Renderer2D
    {
    public:
        static void Initialize(GLFWwindow *window, int width, int height);
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

        static void Clear(const Color &color = Color(0.0f, 0.0f, 0.0f, 1.0f));

        // Tile rendering
        static void DrawTile(const Vec2 &position, const Vec2 &size, const Color &color);

        // Camera access
        static Camera2D &GetCamera() { return s_Camera; }

    private:
        static void InitializeViewPort(GLFWwindow *window, int width, int height);

        // Camera instance
        static Camera2D s_Camera;
    };
}
