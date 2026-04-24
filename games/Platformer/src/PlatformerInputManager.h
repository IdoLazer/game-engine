#pragma once

#include <Engine.h>

// --- Forward Declarations ---
class Player;


class PlatformerInputManager
{
public:
    // --- Constructors & Destructors ---
    PlatformerInputManager() = delete;
    PlatformerInputManager(Player &player);
    ~PlatformerInputManager() = default;

    // --- Input Handling ---
    void HandleKeyPress(const Engine::Key &key);
    void HandleKeyRelease(const Engine::Key &key);

    // --- Fields ---
private:
    Engine::Subscription m_keyPressedSub;
    Engine::Subscription m_keyReleaseSub;
    Player& m_player;
    float m_horizontalInput{0.0f};
};