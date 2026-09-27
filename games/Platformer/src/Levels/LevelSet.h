#pragma once

#include <Engine.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// The level documents of a set, in play order. Its own type so PropertyParser can be specialized
// for it without claiming a vector of strings for every game.
struct LevelPaths
{
    std::vector<std::string> paths;
};

namespace Engine
{
    // One asset path per line of a block value; blank lines and "//" lines are skipped.
    template <>
    struct PropertyParser<LevelPaths>
    {
        static std::optional<LevelPaths> Parse(std::string_view text);
    };
}

// Carries a level list that came from a scene document.
class LevelSet : public Engine::Entity
{
    DECLARE_TYPE(LevelSet, Entity)

    LevelPaths m_levels;

public:
    const std::vector<std::string> &GetLevels() const { return m_levels.paths; }
};

// The level list of a document's LevelSet, read without instantiating anything - the level document
// has to be instantiated before the document naming it, so its tiles render behind everything else.
std::vector<std::string> FindLevelPaths(const Engine::SceneData &document);
