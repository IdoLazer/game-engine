#pragma once

#include <Engine.h>
#include "../Player.h"

// Player inputs that can arrive before the player can act on them: a jump
// pressed just before landing, a glide held during a wall jump. Whoever defers
// one queues the command and runs it when its moment comes.

class JumpCommand : public Engine::Command
{
public:
    explicit JumpCommand(Player &player) : m_player(player) {}
    void Execute() override { m_player.Jump(); }
    const char *GetName() const override { return "JumpCommand"; }
private:
    Player &m_player;
};

class JumpStopCommand : public Engine::Command
{
public:
    explicit JumpStopCommand(Player &player) : m_player(player) {}
    void Execute() override { m_player.StopJump(); }
    const char *GetName() const override { return "JumpStopCommand"; }
private:
    Player &m_player;
};

class GlideCommand : public Engine::Command
{
public:
    explicit GlideCommand(Player &player) : m_player(player) {}
    void Execute() override { m_player.Glide(); }
    const char *GetName() const override { return "GlideCommand"; }
private:
    Player &m_player;
};
