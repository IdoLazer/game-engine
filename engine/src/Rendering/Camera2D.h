#pragma once

#include "Math/Vec2.h"

namespace Engine
{
    class Camera2D
    {
    public:
        // --- Constructors & Destructors ---
        Camera2D(float worldHeight = 10.0f);

        // --- Configuration ---
        void SetWindowSize(int pixelWidth, int pixelHeight);
        void SetPosition(const Vec2 &position);

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
        Vec2 GetPosition() const;

    private:
        // --- Fields ---
        float m_worldHeight;
        float m_worldWidth;
        int m_pixelWidth;
        int m_pixelHeight;
        Vec2 m_position{}; // World point shown at the center of the screen
    };
}
