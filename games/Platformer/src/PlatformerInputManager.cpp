#include "PlatformerInputManager.h"
#include "Player.h"

using namespace Engine;

PlatformerInputManager::PlatformerInputManager(Player &player)
    : m_player(player)
{
    // Subscribe to keyboard input events
    m_keyPressedSub = Engine::Keyboard::OnKeyPressed().Subscribe(this, &PlatformerInputManager::HandleKeyPress);
    m_keyReleaseSub = Engine::Keyboard::OnKeyReleased().Subscribe(this, &PlatformerInputManager::HandleKeyRelease);
}

void PlatformerInputManager::HandleKeyPress(const Engine::Key &key)
{
    switch (key)
    {    
    case Key::A:
    case Key::Left:
        m_horizontalInput -= 1.0f;
        m_player.SetDirection(Vec2{m_horizontalInput, 0.0});
        break;
    case Key::D:
    case Key::Right:
        m_horizontalInput += 1.0f;
        m_player.SetDirection(Vec2{m_horizontalInput, 0.0});
        break;
    case Key::Space:
        if (!m_player.IsJumping())
            m_player.Jump();
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
        m_player.SetDirection(Vec2{m_horizontalInput, 0.0});
        break;
    case Key::D:
    case Key::Right:
        m_horizontalInput -= 1.0f;
        m_player.SetDirection(Vec2{m_horizontalInput, 0.0});
        break;
    case Key::Space:
        m_player.StopJump();
        break;
    default:
        break;
    }
}

