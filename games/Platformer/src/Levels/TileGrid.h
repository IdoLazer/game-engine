#pragma once

#include <Engine.h>
#include "../TileType.h"

#include <optional>
#include <string_view>
#include <vector>

// A level's tile rows. Its own type rather than a bare vector of vectors, so it can carry the
// bounds handling and so PropertyParser can be specialized for it without claiming a container
// every other game shares.
class TileGrid
{
// --- Constructors & Destructors ---
public:
    TileGrid() = default;
    explicit TileGrid(std::vector<std::vector<int>> rows);

// --- Accessors ---
public:
    int GetRowCount() const { return static_cast<int>(m_rows.size()); }
    int GetColumnCount() const { return m_rows.empty() ? 0 : static_cast<int>(m_rows[0].size()); }
    bool IsEmpty() const { return m_rows.empty(); }

    // Empty outside the grid, so callers need no bounds check of their own.
    TileType At(int x, int y) const;

// --- Fields ---
private:
    std::vector<std::vector<int>> m_rows;
};

namespace Engine
{
    // Reads the character rows of a TileGrid block value, per TileLegend.
    template <>
    struct PropertyParser<TileGrid>
    {
        static std::optional<TileGrid> Parse(std::string_view text);
    };
}
