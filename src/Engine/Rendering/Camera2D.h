#pragma once

namespace Engine
{
    // Forward declaration
    struct Vec2;

    class Camera2D
    {
    public:
        Camera2D(float worldHeight = 10.0f);

        // Set the window dimensions to calculate world width
        void SetWindowSize(int pixelWidth, int pixelHeight);

        // Get world dimensions
        float GetWorldWidth() const { return m_WorldWidth; }
        float GetWorldHeight() const { return m_WorldHeight; }

        // Convert world coordinates to OpenGL coordinates (-1 to +1)
        float WorldToOpenGLX(float worldX) const;
        float WorldToOpenGLY(float worldY) const;

        // Convert Vec2 world coordinates to OpenGL coordinates
        Vec2 WorldToOpenGL(const Vec2 &worldPos) const;

    private:
        float m_WorldHeight; // World units visible vertically
        float m_WorldWidth;  // Calculated from aspect ratio
        int m_PixelWidth;
        int m_PixelHeight;
    };
}
