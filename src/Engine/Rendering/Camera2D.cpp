#include "Camera2D.h"
#include "Renderer2D.h" // For Vec2 definition
#include <iostream>

namespace Engine
{
    // --- Constructors & Destructors ---

    Camera2D::Camera2D(float worldHeight)
        : m_worldHeight(worldHeight), m_worldWidth(0.0f), m_pixelWidth(0), m_pixelHeight(0)
    {
    }

    // --- Configuration ---

    void Camera2D::SetWindowSize(int pixelWidth, int pixelHeight)
    {
        m_pixelWidth = pixelWidth;
        m_pixelHeight = pixelHeight;

        // Calculate world width based on aspect ratio
        float aspectRatio = static_cast<float>(pixelWidth) / static_cast<float>(pixelHeight);
        m_worldWidth = m_worldHeight * aspectRatio;

        std::cout << "Camera2D: Window " << pixelWidth << "x" << pixelHeight
                  << " -> World " << m_worldWidth << "x" << m_worldHeight << " units (centered at 0,0)" << std::endl;
    }

    // --- Coordinate Conversion ---

    float Camera2D::WorldToOpenGLX(float worldX) const
    {
        // Convert world X (-m_worldWidth/2 to +m_worldWidth/2) to OpenGL X (-1 to +1)
        return (worldX / (m_worldWidth * 0.5f));
    }

    float Camera2D::WorldToOpenGLY(float worldY) const
    {
        // Convert world Y (-m_worldHeight/2 to +m_worldHeight/2) to OpenGL Y (-1 to +1)
        return (worldY / (m_worldHeight * 0.5f));
    }

    Vec2 Camera2D::WorldToOpenGL(const Vec2 &worldPos) const
    {
        return Vec2(WorldToOpenGLX(worldPos.x), WorldToOpenGLY(worldPos.y));
    }

    Vec2 Camera2D::ScreenToWorld(const Vec2 &screenPos) const
    {
        // Convert screen coordinates (pixels) to world coordinates
        float worldX = (screenPos.x / m_pixelWidth) * m_worldWidth - (m_worldWidth * 0.5f);
        float worldY = ((m_pixelHeight - screenPos.y) / m_pixelHeight) * m_worldHeight - (m_worldHeight * 0.5f);
        return Vec2(worldX, worldY);
    }

    Vec2 Camera2D::WorldToScreen(const Vec2 &worldPos) const
    {
        // Convert world coordinates to screen coordinates (pixels)
        float screenX = ((worldPos.x + (m_worldWidth * 0.5f)) / m_worldWidth) * m_pixelWidth;
        float screenY = m_pixelHeight - (((worldPos.y + (m_worldHeight * 0.5f)) / m_worldHeight) * m_pixelHeight);
        return Vec2(screenX, screenY);
    }

    // --- Accessors ---

    float Camera2D::GetWorldWidth() const { return m_worldWidth; }
    float Camera2D::GetWorldHeight() const { return m_worldHeight; }
    int Camera2D::GetPixelWidth() const { return m_pixelWidth; }
    int Camera2D::GetPixelHeight() const { return m_pixelHeight; }
}
