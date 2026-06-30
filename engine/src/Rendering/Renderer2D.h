#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Camera2D.h"
#include "Texture2D.h"
#include "BitmapFont.h"
#include "Graphics/Color.h"
#include "Math/Vec2.h"

namespace Engine
{
    // --- Types ---
    enum LineStyle
    {
        Solid,
        Dashed,
        Dotted
    };

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
        static void DrawLine(const Vec2 &start, const Vec2 &end, const Color &color, float thickness = 0.05f, LineStyle style = Solid);
        static void DrawRectOutline(const Vec2 &position, const Vec2 &size, const Color &color, float thickness = 0.05f);

        // --- Textured Drawing ---
        static void DrawSprite(const Vec2 &position, const Vec2 &size, const Texture2D &texture, const Color &tint = Color::White);
        static void DrawSprite(const Vec2 &position, const Vec2 &size, const Texture2D &texture, const TextureRect &srcRect, const Color &tint = Color::White);

        // --- Text ---
        static void DrawText(const Vec2 &position, const std::string &text, const BitmapFont &font, float charHeight, const Color &color = Color::White);

        // --- Accessors ---
        static Camera2D &GetCamera();

    private:
        // --- Fields ---
        static Camera2D s_Camera;

        // --- Internal ---
        static void InitializeViewPort(GLFWwindow *window, int width, int height);
    };
}
