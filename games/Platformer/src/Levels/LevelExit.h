#pragma once

#include <Engine.h>

#include <string>

// A region of a level that leads to a spawn point in another level.
class LevelExit : public Engine::Entity
{
    DECLARE_TYPE(LevelExit, Entity)

    std::string m_anchor;
    std::string m_targetLevel;
    std::string m_targetSpawn;

public:
    const std::string &GetAnchor() const { return m_anchor; }
    const std::string &GetTargetLevel() const { return m_targetLevel; }
    const std::string &GetTargetSpawn() const { return m_targetSpawn; }
};
