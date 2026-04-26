#pragma once

namespace Engine
{
    // Forward declaration
    struct Vec2;

    class Camera2D
    {
    public:
        // --- Constructors & Destructors ---
        Camera2D(float worldHeight = 10.0f);

        // --- Configuration ---
        void SetWindowSize(int pixelWidth, int pixelHeight);

        // --- Coordinate Conversion ---
        float WorldToOpenGLX(float worldX) const;
        float WorldToOpenGLY(float worldY) const;
        Vec2 WorldToOpenGL(const Vec2 &worldPos) const;
        Vec2 ScreenToWorld(const Vec2 &screenPos) const;
        Vec2 WorldToScreen(const Vec2 &worldPos) const;

        // --- Accessors ---
        float GetWorldWidth() const;
        float GetWorldHeight() const;
        int GetPixelWidth() const;
        int GetPixelHeight() const;

    private:
        // --- Fields ---
        float m_worldHeight;
        float m_worldWidth;
        int m_pixelWidth;
        int m_pixelHeight;
    };
}
