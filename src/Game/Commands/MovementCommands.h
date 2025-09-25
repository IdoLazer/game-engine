#pragma once

#include "Command.h"
#include "../Player.h"
#include "../GameConstants.h"

// Concrete movement commands implementing the Command interface
// Each command encapsulates a specific direction change request

class MoveCommand : public Command
{
    Player *m_player;
    Vec2 m_direction;
    const char *m_name;

public:
    MoveCommand(Player *player, const Vec2 &direction, const char *name)
        : m_player(player), m_direction(direction), m_name(name) {}

    void Execute() override
    {
        // Only set direction if it won't cause immediate reversal
        if (m_player->GetDirection() != -m_direction)
        {
            m_player->SetDirection(m_direction);
        }
    }

    const char *GetName() const override { return m_name; }
};

class MoveUpCommand : public MoveCommand
{
public:
    MoveUpCommand() = delete;
    MoveUpCommand(Player *player) : MoveCommand(player, GameConstants::DIRECTION_UP, "MoveUp") {}
};

class MoveDownCommand : public MoveCommand
{
public:
    MoveDownCommand() = delete;
    MoveDownCommand(Player *player) : MoveCommand(player, GameConstants::DIRECTION_DOWN, "MoveDown") {}
};

class MoveLeftCommand : public MoveCommand
{
public:
    MoveLeftCommand() = delete;
    MoveLeftCommand(Player *player) : MoveCommand(player, GameConstants::DIRECTION_LEFT, "MoveLeft") {}
};

class MoveRightCommand : public MoveCommand
{
public:
    MoveRightCommand() = delete;
    MoveRightCommand(Player *player) : MoveCommand(player, GameConstants::DIRECTION_RIGHT, "MoveRight") {}
};

class MovementCommandConflictResolver : public ICommandConflictResolver
{
public:
    bool IsConflict(Command *newCmd, Command *queuedCmd) override
    {
        // Check all combinations of opposite directions
        // This uses dynamic_cast to check the actual command types

        // Up vs Down
        if ((dynamic_cast<MoveUpCommand *>(newCmd) && dynamic_cast<MoveDownCommand *>(queuedCmd)) ||
            (dynamic_cast<MoveDownCommand *>(newCmd) && dynamic_cast<MoveUpCommand *>(queuedCmd)))
        {
            return true;
        }

        // Left vs Right
        if ((dynamic_cast<MoveLeftCommand *>(newCmd) && dynamic_cast<MoveRightCommand *>(queuedCmd)) ||
            (dynamic_cast<MoveRightCommand *>(newCmd) && dynamic_cast<MoveLeftCommand *>(queuedCmd)))
        {
            return true;
        }

        return false;
    }
};