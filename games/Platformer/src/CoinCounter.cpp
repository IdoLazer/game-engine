#include "CoinCounter.h"
#include "Session.h"

#include <string_view>

namespace
{
    constexpr std::string_view kCountPlaceholder = "{0}";
}

// --- Type Registration ---

BEGIN_TYPE_REGISTER(CoinCounter)
END_TYPE_REGISTER()

// --- Lifecycle ---

void CoinCounter::Initialize()
{
    TextEntity::Initialize();
    m_format = GetText();
}

void CoinCounter::Update(float deltaTime)
{
    TextEntity::Update(deltaTime);

    if (!m_session)
        return;

    std::string count = std::to_string(m_session->GetCollectedCount());
    std::string text = m_format;
    for (std::size_t at = text.find(kCountPlaceholder); at != std::string::npos; at = text.find(kCountPlaceholder, at + count.size()))
        text.replace(at, kCountPlaceholder.size(), count);

    SetText(text);
}
