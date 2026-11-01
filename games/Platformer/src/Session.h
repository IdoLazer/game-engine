#pragma once

#include <map>
#include <set>
#include <string>

// What outlives a level: the coins taken in each level.
class Session
{
// --- Public Interface ---
public:
    void EnterLevel(const std::string &level);

    // About the level last entered.
    void Collect(char anchor);
    bool IsCollected(char anchor) const;

    int GetCollectedCount() const;

// --- Private Fields ---
private:
    std::string m_level;
    std::map<std::string, std::set<char>> m_collected;
};
