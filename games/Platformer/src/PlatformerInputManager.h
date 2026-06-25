#pragma once

#include <Engine.h>

class PlatformerInputManager
{
public:
    // --- Constructors & Destructors ---
    PlatformerInputManager();
    ~PlatformerInputManager() = default;

    // --- Initialization ---
    void NotifyInitialState();

    // --- Events ---
    Engine::EventSubscriber<Engine::Vec2>& OnMove()           { return m_onMove; }
    Engine::EventSubscriber<>&             OnJump()           { return m_onJump; }
    Engine::EventSubscriber<>&             OnJumpStop()       { return m_onJumpStop; }
    Engine::EventSubscriber<>&             OnNextLevel()      { return m_onNextLevel; }
    Engine::EventSubscriber<>&             OnPreviousLevel()  { return m_onPreviousLevel; }
    Engine::EventSubscriber<>&             OnReloadLevel()    { return m_onReloadLevel; }
    Engine::EventSubscriber<Engine::Vec2>& OnCursorMove()    { return m_onCursorMove; }

private:
    // --- Input Handling ---
    void HandleKeyPress(const Engine::Key &key);
    void HandleKeyRelease(const Engine::Key &key);
    void HandleCursorMove(const Engine::Vec2 &position);

    // --- Subscriptions ---
    Engine::Subscription m_keyPressedSub;
    Engine::Subscription m_keyReleaseSub;
    Engine::Subscription m_cursorMoveSub;

    // --- State ---
    float m_horizontalInput{0.0f};

    // --- Events ---
    Engine::Event<Engine::Vec2> m_onMove;
    Engine::Event<>             m_onJump;
    Engine::Event<>             m_onJumpStop;
    Engine::Event<>             m_onNextLevel;
    Engine::Event<>             m_onPreviousLevel;
    Engine::Event<>             m_onReloadLevel;
    Engine::Event<Engine::Vec2> m_onCursorMove;
};
