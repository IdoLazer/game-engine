#pragma once

#include <Engine.h>

#include <string>

class Session;

// A label showing how many coins the session holds. Its Text is a format in which {0} is the count.
class CoinCounter : public Engine::TextEntity
{
    DECLARE_TYPE(CoinCounter, TextEntity)

// --- Lifecycle ---
public:
    void Initialize() override;
    void Update(float deltaTime) override;

// --- Accessors ---
public:
    void SetSession(const Session *session) { m_session = session; }

// --- Private Fields ---
private:
    std::string m_format;
    const Session *m_session{nullptr};
};
