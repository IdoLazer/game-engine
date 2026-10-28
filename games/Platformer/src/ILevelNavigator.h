#pragma once

#include <Engine.h>

#include <optional>
#include <string>

// What a level can ask of the game.
class ILevelNavigator
{
public:
    virtual ~ILevelNavigator() = default;

    // An empty `level` ends the game.
    virtual void GoToLevel(const std::string &level, const std::string &spawnName, const std::optional<Engine::Vec2> &exitCell) = 0;
    virtual void ReloadCurrentLevel() = 0;
};
