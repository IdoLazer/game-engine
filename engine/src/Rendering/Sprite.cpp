#include "Sprite.h"
#include "Renderer2D.h"

namespace Engine
{
    Sprite::Sprite(Texture2D* texture, Color tint)
        : m_texture(texture), m_tint(tint)
    {
        // Default source rect = full texture
        if (m_texture)
        {
            m_srcRect.x = 0.0f;
            m_srcRect.y = 0.0f;
            m_srcRect.width = static_cast<float>(m_texture->GetWidth());
            m_srcRect.height = static_cast<float>(m_texture->GetHeight());
        }
    }

    Sprite::Sprite(Texture2D* texture, const TextureRect& srcRect, Color tint)
        : m_texture(texture), m_srcRect(srcRect), m_tint(tint)
    {
    }

    void Sprite::Render(const Vec2& position, const Vec2& size) const
    {
        if (!m_texture)
            return;

        Renderer2D::DrawSprite(position, size, *m_texture, m_srcRect, m_tint);
    }
}
