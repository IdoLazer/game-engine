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
        // --- Lifecycle ---
        static void Initialize(GLFWwindow *window, int width, int height);
        static void Shutdown();

        // --- Frame ---
        static void BeginFrame();
        static void EndFrame();
        static void Clear(const Color &color = Color(0.0f, 0.0f, 0.0f, 1.0f));

        // --- Drawing ---
        static void DrawTile(const Vec2 &position, const Vec2 &size, const Color &color);
        static void DrawCircle(const Vec2 &center, float radius, const Color &color);
        static void DrawTriangle(const Vec2 &point1, const Vec2 &point2, const Vec2 &point3, const Color &color);
        static void DrawRectOutline(const Vec2 &position, const Vec2 &size, const Color &color, float thickness = 0.05f);

        // --- Accessors ---
        static Camera2D &GetCamera();

    private:
        // --- Fields ---
        static Camera2D s_Camera;

        // --- Internal ---
        static void InitializeViewPort(GLFWwindow *window, int width, int height);
    };
}
