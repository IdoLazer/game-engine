#include "PlatformerInputManager.h"
#include "Falcon.h"
#include "Player.h"

using namespace Engine;

PlatformerInputManager::PlatformerInputManager()
{
    m_keyPressedSub = Keyboard::OnKeyPressed().Subscribe(this, &PlatformerInputManager::HandleKeyPress);
    m_keyReleaseSub = Keyboard::OnKeyReleased().Subscribe(this, &PlatformerInputManager::HandleKeyRelease);
    m_mouseButtonPressedSub = Mouse::OnButtonPressed().Subscribe(this, &PlatformerInputManager::HandleClick);
    m_mouseButtonReleasedSub = Mouse::OnButtonReleased().Subscribe(this, &PlatformerInputManager::HandleRelease);
}

// --- Level Bindings ---

void PlatformerInputManager::Bind(Player &player, Falcon &falcon)
{
    m_bindings.push_back(m_onMove.Subscribe(&player, &Player::SetDirection));
    m_bindings.push_back(m_onJump.Subscribe(&player, &Player::Jump));
    m_bindings.push_back(m_onJumpStop.Subscribe(&player, &Player::StopJump));
    m_bindings.push_back(m_onGlide.Subscribe(&player, &Player::Glide));
    m_bindings.push_back(m_onStopGlide.Subscribe(&player, &Player::StopGlide));

    m_bindings.push_back(m_onAim.Subscribe(&falcon, &Falcon::StartAiming));
    m_bindings.push_back(m_onRelease.Subscribe(&falcon, &Falcon::ReleaseAiming));
    m_bindings.push_back(m_onRetrieve.Subscribe(&falcon, &Falcon::Retrieve));

    // Read from the keyboard, not the event history: a key held since before the window
    // opened never sent its press.
    m_horizontalInput = ReadHorizontalInput();
    m_onMove.Notify(Vec2{m_horizontalInput, 0.0f});
}

void PlatformerInputManager::Unbind()
{
    m_bindings.clear();
}

// --- Input Handling ---

float PlatformerInputManager::ReadHorizontalInput() const
{
    float input = 0.0f;
    if (Keyboard::IsKeyDown(Key::A) || Keyboard::IsKeyDown(Key::Left))
        input -= 1.0f;
    if (Keyboard::IsKeyDown(Key::D) || Keyboard::IsKeyDown(Key::Right))
        input += 1.0f;
    return input;
}

void PlatformerInputManager::HandleKeyPress(const Key &key)
{
    switch (key)
    {
    case Key::A:
    case Key::Left:
        m_horizontalInput -= 1.0f;
        m_onMove.Notify(Vec2{m_horizontalInput, 0.0f});
        break;
    case Key::D:
    case Key::Right:
        m_horizontalInput += 1.0f;
        m_onMove.Notify(Vec2{m_horizontalInput, 0.0f});
        break;
    case Key::Space:
        m_onJump.Notify();
        break;
    case Key::N:
        m_onNextLevel.Notify();
        break;
    case Key::P:
        m_onPreviousLevel.Notify();
        break;
    case Key::R:
        m_onReloadLevel.Notify();
        break;
    case Key::Escape:
        m_onQuit.Notify();
        break;
    case Key::LeftShift:
        m_onGlide.Notify();
        break;
    default:
        break;
    }
}

void PlatformerInputManager::HandleKeyRelease(const Key &key)
{
    switch (key)
    {
    case Key::A:
    case Key::Left:
        m_horizontalInput += 1.0f;
        m_onMove.Notify(Vec2{m_horizontalInput, 0.0f});
        break;
    case Key::D:
    case Key::Right:
        m_horizontalInput -= 1.0f;
        m_onMove.Notify(Vec2{m_horizontalInput, 0.0f});
        break;
    case Key::Space:
        m_onJumpStop.Notify();
        break;
    case Key::LeftShift:
        m_onStopGlide.Notify();
        break;
    default:
        break;
    }
}

void PlatformerInputManager::HandleClick(const Engine::MouseButton &button)
{
    switch (button)
    {
    case MouseButton::Left:
        m_onAim.Notify();
        break;
    case MouseButton::Right:
        m_onRetrieve.Notify();
        break;
    default:
        break;
    }
}

void PlatformerInputManager::HandleRelease(const Engine::MouseButton &button)
{
    switch (button)
    {
    case MouseButton::Left:
        m_onRelease.Notify();
        break;
    default:
        break;
    }
}
