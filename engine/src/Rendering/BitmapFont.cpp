#include "BitmapFont.h"
#include <iostream>

namespace Engine
{
    BitmapFont::BitmapFont(const std::string& filePath)
        : m_atlas(filePath)
    {
        m_path = filePath;
        m_cellWidth = m_atlas.GetWidth() / COLUMNS;
        m_cellHeight = m_atlas.GetHeight() / ROWS;
    }

    TextureRect BitmapFont::GetGlyphRect(char c) const
    {
        int index = static_cast<int>(c) - static_cast<int>(FIRST_CHAR);

        int totalGlyphs = COLUMNS * ROWS;
        if (index < 0 || index >= totalGlyphs)
            return {0.0f, 0.0f, 0.0f, 0.0f};

        int col = index % COLUMNS;
        int row = index / COLUMNS;

        return {
            static_cast<float>(col * m_cellWidth),
            static_cast<float>(row * m_cellHeight),
            static_cast<float>(m_cellWidth),
            static_cast<float>(m_cellHeight)
        };
    }

    float BitmapFont::GetAspectRatio() const
    {
        if (m_cellHeight == 0)
            return 1.0f;
        return static_cast<float>(m_cellWidth) / static_cast<float>(m_cellHeight);
    }
}
