#include "PlatformerInputManager.h"

using namespace Engine;

PlatformerInputManager::PlatformerInputManager()
{
    if (Keyboard::IsKeyDown(Key::A) || Keyboard::IsKeyDown(Key::Left))
        m_horizontalInput -= 1.0f;
    if (Keyboard::IsKeyDown(Key::D) || Keyboard::IsKeyDown(Key::Right))
        m_horizontalInput += 1.0f;

    m_keyPressedSub = Keyboard::OnKeyPressed().Subscribe(this, &PlatformerInputManager::HandleKeyPress);
    m_keyReleaseSub = Keyboard::OnKeyReleased().Subscribe(this, &PlatformerInputManager::HandleKeyRelease);
    m_cursorMoveSub = Mouse::OnMoved().Subscribe(this, &PlatformerInputManager::HandleCursorMove);
    m_mouseButtonPressedSub = Mouse::OnButtonPressed().Subscribe(this, &PlatformerInputManager::HandleClick);
    m_mouseButtonReleasedSub = Mouse::OnButtonReleased().Subscribe(this, &PlatformerInputManager::HandleRelease);
}

void PlatformerInputManager::NotifyInitialState()
{
    m_onMove.Notify(Vec2{m_horizontalInput, 0.0f});
    m_onCursorMove.Notify(Mouse::GetWorldPosition());
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
    default:
        break;
    }
}

void PlatformerInputManager::HandleCursorMove(const Vec2 &position)
{
    m_onCursorMove.Notify(position);
}

void PlatformerInputManager::HandleClick(const Engine::MouseButton &button)
{
    switch (button)
    {
    case MouseButton::Left:
        m_onAim.Notify();
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
