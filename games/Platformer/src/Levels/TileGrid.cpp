#include "TileGrid.h"
#include "TileLegend.h"

#include <algorithm>
#include <iostream>
#include <limits>

TileGrid::TileGrid(std::vector<std::vector<int>> rows, std::map<char, Engine::Rect> anchors)
    : m_rows(std::move(rows)), m_anchors(std::move(anchors))
{
}

TileType TileGrid::At(int x, int y) const
{
    if (y < 0 || y >= GetRowCount() || x < 0 || x >= GetColumnCount())
        return TileType::Empty;

    return static_cast<TileType>(m_rows[y][x]);
}

std::optional<Engine::Rect> TileGrid::FindAnchor(char glyph) const
{
    auto anchor = m_anchors.find(glyph);
    if (anchor == m_anchors.end())
        return std::nullopt;

    return anchor->second;
}

namespace Engine
{
    namespace
    {
        // The smallest and largest column and row a glyph is drawn in.
        struct CellExtent
        {
            int minX{std::numeric_limits<int>::max()};
            int minY{std::numeric_limits<int>::max()};
            int maxX{std::numeric_limits<int>::min()};
            int maxY{std::numeric_limits<int>::min()};

            void Include(int x, int y)
            {
                minX = std::min(minX, x);
                minY = std::min(minY, y);
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);
            }
        };

        // A cell at column x, row y is centered on x, y and reaches half a cell in each direction.
        Rect BoundingRect(const CellExtent &extent)
        {
            return Rect(Vec2((extent.minX + extent.maxX) * 0.5f, (extent.minY + extent.maxY) * 0.5f),
                        Vec2((extent.maxX - extent.minX + 1) * 0.5f, (extent.maxY - extent.minY + 1) * 0.5f));
        }
    }

    std::optional<TileGrid> PropertyParser<TileGrid>::Parse(std::string_view text)
    {
        if (text.empty())
            return std::nullopt;

        std::vector<std::vector<int>> rows;
        std::map<char, CellExtent> extents;
        std::size_t lineStart = 0;
        int lineNumber = 0;

        while (lineStart <= text.size())
        {
            std::size_t newline = text.find('\n', lineStart);
            std::string_view line = text.substr(lineStart, newline == std::string_view::npos
                                                              ? std::string_view::npos
                                                              : newline - lineStart);
            ++lineNumber;

            std::vector<int> row;
            row.reserve(line.size());

            for (std::size_t column = 0; column < line.size(); ++column)
            {
                char glyph = line[column];

                if (TileLegend::IsAnchor(glyph))
                {
                    extents[glyph].Include(static_cast<int>(column), static_cast<int>(rows.size()));
                    row.push_back(static_cast<int>(TileType::Empty));
                    continue;
                }

                std::optional<TileType> tile = TileLegend::FromChar(glyph);
                if (!tile)
                {
                    std::cerr << "TileGrid: row " << lineNumber << ", column " << column + 1
                              << ": '" << glyph << "' is not in the tile legend" << std::endl;
                    return std::nullopt;
                }
                row.push_back(static_cast<int>(*tile));
            }

            rows.push_back(std::move(row));

            if (newline == std::string_view::npos)
                break;
            lineStart = newline + 1;
        }

        // Short rows are padded rather than rejected, so trailing empty tiles need not be typed out.
        std::size_t widest = 0;
        for (const std::vector<int> &row : rows)
            widest = std::max(widest, row.size());

        if (widest == 0)
            return std::nullopt;

        for (std::vector<int> &row : rows)
            row.resize(widest, static_cast<int>(TileType::Empty));

        std::map<char, Rect> anchors;
        for (const auto &[glyph, extent] : extents)
            anchors.emplace(glyph, BoundingRect(extent));

        return TileGrid(std::move(rows), std::move(anchors));
    }
}
