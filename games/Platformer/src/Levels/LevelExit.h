#pragma once

#include "LevelRegion.h"

#include <string>

// A region of a level that leads to a spawn point in another level.
class LevelExit : public LevelRegion
{
    DECLARE_TYPE(LevelExit, LevelRegion)

    std::string m_targetLevel;
    std::string m_targetSpawn;

public:
    const std::string &GetTargetLevel() const { return m_targetLevel; }
    const std::string &GetTargetSpawn() const { return m_targetSpawn; }
};
