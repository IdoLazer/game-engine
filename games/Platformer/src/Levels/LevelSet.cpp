#include "LevelSet.h"

#include <Utilities/StringUtils.h>

BEGIN_TYPE_REGISTER(LevelSet)
    REGISTER_PROPERTY(LevelPaths, Levels, &LevelSet::m_levels)
END_TYPE_REGISTER()

namespace Engine
{
    std::optional<LevelPaths> PropertyParser<LevelPaths>::Parse(std::string_view text)
    {
        LevelPaths levels;
        std::size_t lineStart = 0;

        while (lineStart <= text.size())
        {
            std::size_t newline = text.find('\n', lineStart);
            std::string_view line = Trim(text.substr(lineStart, newline == std::string_view::npos
                                                                    ? std::string_view::npos
                                                                    : newline - lineStart));

            if (!line.empty() && !line.starts_with("//"))
                levels.paths.emplace_back(line);

            if (newline == std::string_view::npos)
                break;
            lineStart = newline + 1;
        }

        if (levels.paths.empty())
            return std::nullopt;

        return levels;
    }
}

std::vector<std::string> FindLevelPaths(const Engine::SceneData &document)
{
    for (const Engine::Scene::EntityInfo &info : document.GetEntities())
    {
        if (info.typeName != LevelSet::GetStaticTypeName())
            continue;

        auto property = info.properties.find("Levels");
        if (property == info.properties.end())
            continue;

        if (const LevelPaths *levels = std::any_cast<LevelPaths>(&property->second))
            return levels->paths;
    }

    return {};
}
