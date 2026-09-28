#pragma once

#include <string_view>

namespace Engine
{
    inline constexpr std::string_view WHITESPACE = " \t";

    inline std::string_view Trim(std::string_view text)
    {
        std::size_t first = text.find_first_not_of(WHITESPACE);
        if (first == std::string_view::npos)
            return {};

        std::size_t last = text.find_last_not_of(WHITESPACE);
        return text.substr(first, last - first + 1);
    }

    // Leading whitespace is kept, so a line of a block value keeps its alignment.
    inline std::string_view TrimEnd(std::string_view text)
    {
        std::size_t last = text.find_last_not_of(WHITESPACE);
        if (last == std::string_view::npos)
            return {};

        return text.substr(0, last + 1);
    }
}
