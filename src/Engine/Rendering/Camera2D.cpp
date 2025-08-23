#include "Camera2D.h"
#include "Renderer2D.h" // For Vec2 definition
#include <iostream>

namespace Engine
{
    Camera2D::Camera2D(float worldHeight)
        : m_WorldHeight(worldHeight), m_WorldWidth(0.0f), m_PixelWidth(0), m_PixelHeight(0)
    {
    }

    void Camera2D::SetWindowSize(int pixelWidth, int pixelHeight)
    {
        m_PixelWidth = pixelWidth;
        m_PixelHeight = pixelHeight;

        // Calculate world width based on aspect ratio
        float aspectRatio = static_cast<float>(pixelWidth) / static_cast<float>(pixelHeight);
        m_WorldWidth = m_WorldHeight * aspectRatio;

        std::cout << "Camera2D: Window " << pixelWidth << "x" << pixelHeight
                  << " -> World " << m_WorldWidth << "x" << m_WorldHeight << " units (centered at 0,0)" << std::endl;
    }

    float Camera2D::WorldToOpenGLX(float worldX) const
    {
        // Convert world X (-m_WorldWidth/2 to +m_WorldWidth/2) to OpenGL X (-1 to +1)
        return (worldX / (m_WorldWidth * 0.5f));
    }

    float Camera2D::WorldToOpenGLY(float worldY) const
    {
        // Convert world Y (-m_WorldHeight/2 to +m_WorldHeight/2) to OpenGL Y (-1 to +1)
        return (worldY / (m_WorldHeight * 0.5f));
    }

    Vec2 Camera2D::WorldToOpenGL(const Vec2 &worldPos) const
    {
        return Vec2(WorldToOpenGLX(worldPos.x), WorldToOpenGLY(worldPos.y));
    }
}
