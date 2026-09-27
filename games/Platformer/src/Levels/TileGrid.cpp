#include "TileGrid.h"
#include "TileLegend.h"

#include <algorithm>
#include <iostream>

TileGrid::TileGrid(std::vector<std::vector<int>> rows)
    : m_rows(std::move(rows))
{
}

TileType TileGrid::At(int x, int y) const
{
    if (y < 0 || y >= GetRowCount() || x < 0 || x >= GetColumnCount())
        return TileType::Empty;

    return static_cast<TileType>(m_rows[y][x]);
}

namespace Engine
{
    std::optional<TileGrid> PropertyParser<TileGrid>::Parse(std::string_view text)
    {
        if (text.empty())
            return std::nullopt;

        std::vector<std::vector<int>> rows;
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
                std::optional<TileType> tile = TileLegend::FromChar(line[column]);
                if (!tile)
                {
                    std::cerr << "TileGrid: row " << lineNumber << ", column " << column + 1
                              << ": '" << line[column] << "' is not in the tile legend" << std::endl;
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

        return TileGrid(std::move(rows));
    }
}
