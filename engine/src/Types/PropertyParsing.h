#pragma once

#include <any>
#include <concepts>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../Graphics/Color.h"
#include "../Math/Vec2.h"
#include "../Utilities/StringUtils.h"

namespace Engine
{
    // Turns a scene file's text into a property value. Specialize it to make properties of your own
    // type settable from a file:
    //   template <> struct Engine::PropertyParser<MyType>
    //   { static std::optional<MyType> Parse(std::string_view text); };
    template <typename T>
    struct PropertyParser;

    template <typename T>
    concept ParsableProperty = requires(std::string_view text) {
        { PropertyParser<T>::Parse(text) } -> std::same_as<std::optional<T>>;
    };

    template <> struct PropertyParser<float>       { static std::optional<float> Parse(std::string_view text); };
    template <> struct PropertyParser<int>         { static std::optional<int> Parse(std::string_view text); };
    template <> struct PropertyParser<bool>        { static std::optional<bool> Parse(std::string_view text); };
    template <> struct PropertyParser<std::string> { static std::optional<std::string> Parse(std::string_view text); };
    template <> struct PropertyParser<Vec2>        { static std::optional<Vec2> Parse(std::string_view text); };
    template <> struct PropertyParser<Color>       { static std::optional<Color> Parse(std::string_view text); };

    // One element per line, since an element such as a Vec2 has commas of its own.
    template <ParsableProperty T>
    struct PropertyParser<std::vector<T>>
    {
        static std::optional<std::vector<T>> Parse(std::string_view text)
        {
            std::vector<T> elements;

            for (std::string_view line : SplitNonBlankLines(text))
            {
                std::optional<T> element = PropertyParser<T>::Parse(line);
                if (!element)
                    return std::nullopt;

                elements.push_back(std::move(*element));
            }

            return elements;
        }
    };

    // Null for a type with no PropertyParser - properties of that type stay assigned in code.
    template <typename T>
    std::function<std::any(std::string_view)> MakePropertyParser()
    {
        if constexpr (ParsableProperty<T>)
        {
            return [](std::string_view text) -> std::any
            {
                std::optional<T> parsed = PropertyParser<T>::Parse(text);
                return parsed.has_value() ? std::any(*parsed) : std::any{};
            };
        }
        else
        {
            return nullptr;
        }
    }
}
