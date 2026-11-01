#include "Session.h"

// --- Public Interface ---

void Session::EnterLevel(const std::string &level)
{
    m_level = level;
}

void Session::Collect(char anchor)
{
    m_collected[m_level].insert(anchor);
}

bool Session::IsCollected(char anchor) const
{
    auto level = m_collected.find(m_level);
    return level != m_collected.end() && level->second.contains(anchor);
}

int Session::GetCollectedCount() const
{
    int count = 0;
    for (const auto &[level, anchors] : m_collected)
        count += static_cast<int>(anchors.size());

    return count;
}
