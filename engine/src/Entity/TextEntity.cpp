#include "TextEntity.h"
#include "Rendering/Renderer2D.h"
#include "Rendering/BitmapFont.h"
#include "Resources/ResourceManager.h"

namespace Engine
{
    BEGIN_TYPE_REGISTER(TextEntity)
        REGISTER_PROPERTY(std::string, FontPath, &TextEntity::m_fontPath)
        REGISTER_PROPERTY(std::string, Text, &TextEntity::m_text)
        REGISTER_PROPERTY(float, CharHeight, &TextEntity::m_charHeight)
        REGISTER_PROPERTY(Color, BackgroundColor, &TextEntity::m_backgroundColor)
    END_TYPE_REGISTER()

    void TextEntity::Initialize()
    {
        if (!m_fontPath.empty() && !m_font)
            m_font = ResourceManager::Load<BitmapFont>(m_fontPath);
    }

    void TextEntity::Render() const
    {
        if (!m_font || m_text.empty())
            return;

        // Draw background tile if not fully transparent
        if (m_backgroundColor.a > 0.0f)
        {
            float charWidth = m_charHeight * m_font->GetAspectRatio();
            float totalWidth = charWidth * static_cast<float>(m_text.size());
            Vec2 bgSize(totalWidth, m_charHeight);
            Vec2 bgCenter(m_worldPosition.x + totalWidth * 0.5f, m_worldPosition.y - m_charHeight * 0.5f);
            Renderer2D::DrawTile(bgCenter, bgSize, m_backgroundColor);
        }

        Renderer2D::DrawText(m_worldPosition, m_text, *m_font, m_charHeight, m_color);
    }
}
