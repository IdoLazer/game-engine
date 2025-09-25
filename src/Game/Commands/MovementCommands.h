#pragma once

#include <Engine.h>
#include "../GameConstants.h"

class IMovable
{
public:
    virtual ~IMovable() = default;

    virtual void SetDirection(const Vec2 &direction) = 0;
};

// Concrete movement commands implementing the Command interface
// Each command encapsulates a specific direction change request

class MoveCommand : public Engine::Command
{
    IMovable *m_movable;
    Vec2 m_direction;
    const char *m_name;

public:
    MoveCommand(IMovable *movable, const Vec2 &direction, const char *name)
        : m_movable(movable), m_direction(direction), m_name(name) {}

    void Execute() override
    {
        m_movable->SetDirection(m_direction);
    }

    const char *GetName() const override { return m_name; }
};

class MoveUpCommand : public MoveCommand
{
public:
    MoveUpCommand() = delete;
    MoveUpCommand(IMovable *movable) : MoveCommand(movable, GameConstants::DIRECTION_UP, "MoveUp") {}
};

class MoveDownCommand : public MoveCommand
{
public:
    MoveDownCommand() = delete;
    MoveDownCommand(IMovable *movable) : MoveCommand(movable, GameConstants::DIRECTION_DOWN, "MoveDown") {}
};

class MoveLeftCommand : public MoveCommand
{
public:
    MoveLeftCommand() = delete;
    MoveLeftCommand(IMovable *movable) : MoveCommand(movable, GameConstants::DIRECTION_LEFT, "MoveLeft") {}
};

class MoveRightCommand : public MoveCommand
{
public:
    MoveRightCommand() = delete;
    MoveRightCommand(IMovable *movable) : MoveCommand(movable, GameConstants::DIRECTION_RIGHT, "MoveRight") {}
};

class MovementCommandConflictResolver : public Engine::ICommandConflictResolver
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