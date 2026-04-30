#pragma once

#include "Texture2D.h"
#include "Graphics/Color.h"
#include "Math/Vec2.h"

namespace Engine
{
    // A renderable textured quad. Combines a texture reference, a source
    // rectangle (for sprite sheets), and a tint color.
    //
    // This is a data-oriented class meant to be stored on entities as a
    // precursor to a future SpriteComponent in an ECS.
    class Sprite
    {
    public:
        Sprite() = default;

        // Construct a sprite using the full texture.
        explicit Sprite(Texture2D* texture, Color tint = Color::White);

        // Construct a sprite using a sub-region of the texture (sprite sheet).
        Sprite(Texture2D* texture, const TextureRect& srcRect, Color tint = Color::White);

        // Draw the sprite at the given world position and size.
        void Render(const Vec2& position, const Vec2& size) const;

        // --- Accessors ---
        Texture2D* GetTexture() const { return m_texture; }
        const TextureRect& GetSourceRect() const { return m_srcRect; }
        const Color& GetTint() const { return m_tint; }

        void SetTexture(Texture2D* texture) { m_texture = texture; }
        void SetSourceRect(const TextureRect& srcRect) { m_srcRect = srcRect; }
        void SetTint(const Color& tint) { m_tint = tint; }

    private:
        Texture2D* m_texture = nullptr;
        TextureRect m_srcRect;             // Source region (defaults to full texture)
        Color m_tint = Color::White;
    };
}
