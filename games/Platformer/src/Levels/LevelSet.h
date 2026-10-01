#pragma once

#include <Engine.h>

#include <string>
#include <vector>

// The level documents of a set, in play order.
class LevelSet : public Engine::Entity
{
    DECLARE_TYPE(LevelSet, Entity)

    std::vector<std::string> m_levels;

public:
    const std::vector<std::string> &GetLevels() const { return m_levels; }
};
