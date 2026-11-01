#pragma once

#include <string>
#include "Entity.h"

namespace Engine
{
    class BitmapFont;

    // An entity that renders a text string using a BitmapFont.
    // Position is the top-left corner of the text in world coordinates, unless ScreenSpace is set:
    // then ScreenPosition (pixels from the window's top-left) places it, however the camera moves.
    class TextEntity : public Entity
    {
        DECLARE_TYPE(TextEntity, Entity)

    // --- Fields ---
    private:
        BitmapFont* m_font = nullptr;
        std::string m_fontPath;
        std::string m_text;
        float m_charHeight = 0.5f;
        Color m_backgroundColor = Color::Transparent;
        bool m_screenSpace = false;
        Vec2 m_screenPosition = Vec2::Zero;
    
    // --- Lifecycle ---
    public:
        void Initialize() override;
        void Update(float deltaTime) override;
        void Render() const override;

    // --- Accessors ---
    public:
        void SetFont(BitmapFont* font) { m_font = font; }
        void SetText(const std::string& text) { m_text = text; }
        void SetCharHeight(float height) { m_charHeight = height; }
        void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }

        BitmapFont* GetFont() const { return m_font; }
        const std::string& GetText() const { return m_text; }
        float GetCharHeight() const { return m_charHeight; }
        const Color& GetBackgroundColor() const { return m_backgroundColor; }
    
    };
}
