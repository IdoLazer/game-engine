#include "PlatformerInputManager.h"

using namespace Engine;

PlatformerInputManager::PlatformerInputManager()
{
    if (Engine::Keyboard::IsKeyDown(Key::A) || Engine::Keyboard::IsKeyDown(Key::Left))
        m_horizontalInput -= 1.0f;
    if (Engine::Keyboard::IsKeyDown(Key::D) || Engine::Keyboard::IsKeyDown(Key::Right))
        m_horizontalInput += 1.0f;

    m_keyPressedSub = Keyboard::OnKeyPressed().Subscribe(this, &PlatformerInputManager::HandleKeyPress);
    m_keyReleaseSub = Keyboard::OnKeyReleased().Subscribe(this, &PlatformerInputManager::HandleKeyRelease);
}

void PlatformerInputManager::NotifyInitialState()
{
    m_onMove.Notify(Vec2{m_horizontalInput, 0.0f});
}

void PlatformerInputManager::HandleKeyPress(const Engine::Key &key)
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
    default:
        break;
    }
}

void PlatformerInputManager::HandleKeyRelease(const Engine::Key &key)
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
    default:
        break;
    }
}
