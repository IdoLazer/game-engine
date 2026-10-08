#pragma once

#include <Engine.h>

#include <string>

// A named place in a level where the player can appear.
class SpawnPoint : public Engine::Entity
{
    DECLARE_TYPE(SpawnPoint, Entity)

    std::string m_name;
    std::string m_anchor;

public:
    const std::string &GetName() const { return m_name; }
    const std::string &GetAnchor() const { return m_anchor; }
};
