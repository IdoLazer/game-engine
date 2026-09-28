#include "PropertyParsing.h"
#include "../Utilities/StringUtils.h"

#include <cstdlib>
#include <vector>

namespace Engine
{
    namespace
    {
        std::vector<std::string_view> SplitOnCommas(std::string_view text)
        {
            std::vector<std::string_view> fields;
            std::size_t start = 0;

            while (true)
            {
                std::size_t comma = text.find(',', start);
                if (comma == std::string_view::npos)
                {
                    fields.push_back(Trim(text.substr(start)));
                    return fields;
                }

                fields.push_back(Trim(text.substr(start, comma - start)));
                start = comma + 1;
            }
        }

        // Both numeric parsers reject trailing junk: the whole field has to be the number, so
        // "12x" fails rather than quietly reading 12.
        std::optional<float> ToFloat(std::string_view text)
        {
            std::string field(Trim(text));
            if (field.empty())
                return std::nullopt;

            char *end = nullptr;
            float value = std::strtof(field.c_str(), &end);
            if (end != field.c_str() + field.size())
                return std::nullopt;

            return value;
        }

        std::optional<long> ToLong(std::string_view text)
        {
            std::string field(Trim(text));
            if (field.empty())
                return std::nullopt;

            char *end = nullptr;
            long value = std::strtol(field.c_str(), &end, 10);
            if (end != field.c_str() + field.size())
                return std::nullopt;

            return value;
        }
    }

    std::optional<float> PropertyParser<float>::Parse(std::string_view text)
    {
        return ToFloat(text);
    }

    std::optional<int> PropertyParser<int>::Parse(std::string_view text)
    {
        std::optional<long> value = ToLong(text);
        if (!value)
            return std::nullopt;

        return static_cast<int>(*value);
    }

    std::optional<bool> PropertyParser<bool>::Parse(std::string_view text)
    {
        std::string_view field = Trim(text);
        if (field == "true")
            return true;
        if (field == "false")
            return false;

        return std::nullopt;
    }

    std::optional<std::string> PropertyParser<std::string>::Parse(std::string_view text)
    {
        return std::string(Trim(text));
    }

    std::optional<Vec2> PropertyParser<Vec2>::Parse(std::string_view text)
    {
        std::vector<std::string_view> fields = SplitOnCommas(text);
        if (fields.size() != 2)
            return std::nullopt;

        std::optional<float> x = ToFloat(fields[0]);
        std::optional<float> y = ToFloat(fields[1]);
        if (!x || !y)
            return std::nullopt;

        return Vec2(*x, *y);
    }

    // Alpha is optional and defaults to opaque, so "0.3, 0.3, 0.3" is a valid colour.
    std::optional<Color> PropertyParser<Color>::Parse(std::string_view text)
    {
        std::vector<std::string_view> fields = SplitOnCommas(text);
        if (fields.size() != 3 && fields.size() != 4)
            return std::nullopt;

        float components[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        for (std::size_t i = 0; i < fields.size(); ++i)
        {
            std::optional<float> value = ToFloat(fields[i]);
            if (!value)
                return std::nullopt;

            components[i] = *value;
        }

        return Color(components[0], components[1], components[2], components[3]);
    }
}
