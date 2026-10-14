#pragma once

#include "../TileType.h"
#include <optional>

// The characters a scene document's TileGrid is written in. This table is the legend a level
// author reads, so it lives in the header rather than behind a lookup function.
namespace TileLegend
{
    struct Entry
    {
        char glyph;
        TileType tile;
    };

    inline constexpr Entry ENTRIES[] = {
        {'.', TileType::Empty},
        {'#', TileType::Solid},
        {'x', TileType::Death},
    };

    constexpr std::optional<TileType> FromChar(char glyph)
    {
        for (const Entry &entry : ENTRIES)
        {
            if (entry.glyph == glyph)
                return entry.tile;
        }
        return std::nullopt;
    }

    // Any other visible character is an anchor: an empty cell an entity names to say where it is.
    constexpr bool IsAnchor(char glyph)
    {
        return glyph > ' ' && glyph <= '~' && !FromChar(glyph);
    }
}
