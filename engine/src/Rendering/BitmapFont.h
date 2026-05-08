#pragma once

#include <string>
#include "Texture2D.h"
#include "Resources/Resource.h"

namespace Engine
{
    // A monospace bitmap font loaded from a grid-based character atlas.
    //
    // The atlas PNG contains characters arranged in a 16-column × 6-row grid,
    // starting at ASCII 32 (space) through ASCII 127 (~). This is the de facto
    // standard layout for ASCII font atlases.
    //
    // Loaded as a Resource via ResourceManager:
    //   auto* font = ResourceManager::Load<BitmapFont>("assets/font.png");
    class BitmapFont : public Resource
    {
    public:
        // Loads the font atlas from disk and derives cell dimensions.
        explicit BitmapFont(const std::string& filePath);

        // Get the source rectangle (in pixels) for a given character.
        // Returns a zero-size rect for characters outside the atlas range.
        TextureRect GetGlyphRect(char c) const;

        // --- Accessors ---
        Texture2D* GetTexture() { return &m_atlas; }
        const Texture2D* GetTexture() const { return &m_atlas; }
        int GetCellWidth() const { return m_cellWidth; }
        int GetCellHeight() const { return m_cellHeight; }

        // Width-to-height ratio of a single character cell.
        float GetAspectRatio() const;

    private:
        static constexpr int COLUMNS = 16;
        static constexpr int ROWS = 6;
        static constexpr char FIRST_CHAR = ' ';  // ASCII 32

        Texture2D m_atlas;
        int m_cellWidth = 0;
        int m_cellHeight = 0;
    };
}
