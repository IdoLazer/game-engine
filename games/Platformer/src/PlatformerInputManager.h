#pragma once

#include <Engine.h>

// --- Forward Declarations ---
class Player;
class Falcon;
class Cursor;

class PlatformerInputManager
{
public:
    // --- Constructors & Destructors ---
    PlatformerInputManager();
    ~PlatformerInputManager() = default;

    // --- Level Bindings ---
    // Routes input to a level's entities until Unbind. `cursor` may be null.
    void Bind(Player &player, Falcon &falcon, Cursor *cursor);
    void Unbind();

    // --- Events ---
    Engine::EventSubscriber<void(Engine::Vec2)>& OnMove()           { return m_onMove; }
    Engine::EventSubscriber<void()>&             OnJump()           { return m_onJump; }
    Engine::EventSubscriber<void()>&             OnJumpStop()       { return m_onJumpStop; }
    Engine::EventSubscriber<void()>&             OnNextLevel()      { return m_onNextLevel; }
    Engine::EventSubscriber<void()>&             OnPreviousLevel()  { return m_onPreviousLevel; }
    Engine::EventSubscriber<void()>&             OnReloadLevel()    { return m_onReloadLevel; }
    Engine::EventSubscriber<void()>&             OnQuit()           { return m_onQuit; }
    Engine::EventSubscriber<void(Engine::Vec2)>& OnCursorMove()     { return m_onCursorMove; }
    Engine::EventSubscriber<void()>&             OnAim()            { return m_onAim; }
    Engine::EventSubscriber<void()>&             OnRelease()        { return m_onRelease; }
    Engine::EventSubscriber<void()>&             OnRetrieve()       { return m_onRetrieve; }
    Engine::EventSubscriber<void()>&             OnGlide()          { return m_onGlide; }
    Engine::EventSubscriber<void()>&             OnStopGlide()      { return m_onStopGlide; }

private:
    // --- Input Handling ---
    float ReadHorizontalInput() const;
    void HandleKeyPress(const Engine::Key &key);
    void HandleKeyRelease(const Engine::Key &key);
    void HandleCursorMove(const Engine::Vec2 &position);
    void HandleClick(const Engine::MouseButton &button);
    void HandleRelease(const Engine::MouseButton &button);

    // --- Subscriptions ---
    Engine::Subscription m_keyPressedSub;
    Engine::Subscription m_keyReleaseSub;
    Engine::Subscription m_cursorMoveSub;
    Engine::Subscription m_mouseButtonPressedSub;
    Engine::Subscription m_mouseButtonReleasedSub;
    std::vector<Engine::Subscription> m_bindings;

    // --- State ---
    float m_horizontalInput{0.0f};

    // --- Events ---
    Engine::Event<void(Engine::Vec2)> m_onMove;
    Engine::Event<void()>             m_onJump;
    Engine::Event<void()>             m_onJumpStop;
    Engine::Event<void()>             m_onNextLevel;
    Engine::Event<void()>             m_onPreviousLevel;
    Engine::Event<void()>             m_onReloadLevel;
    Engine::Event<void()>             m_onQuit;
    Engine::Event<void(Engine::Vec2)> m_onCursorMove;
    Engine::Event<void()>             m_onAim;
    Engine::Event<void()>             m_onRelease;
    Engine::Event<void()>             m_onRetrieve;
    Engine::Event<void()>             m_onGlide;
    Engine::Event<void()>             m_onStopGlide;
};
