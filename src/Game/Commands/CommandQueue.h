#pragma once

#include "Command.h"
#include <memory>
#include <queue>

// Interface for resolving command conflicts
class ICommandConflictResolver
{
public:
    virtual ~ICommandConflictResolver() = default;

    // Check if two commands are in conflict
    virtual bool IsConflict(Command *newCmd, Command *queuedCmd) = 0;
};

// CommandQueue manages a limited buffer of commands with intelligent conflict resolution
// Features:
// - Maximum queue size of 2 commands to prevent unresponsiveness
// - Opposite direction commands override conflicting queued commands
// - FIFO execution order for non-conflicting commands

class CommandQueue
{
private:
    static constexpr size_t MAX_QUEUE_SIZE = 2;
    std::queue<std::unique_ptr<Command>> m_commands;

    ICommandConflictResolver *m_conflictResolver{nullptr};

public:
    CommandQueue() = default;
    ~CommandQueue() = default;

    // Non-copyable since we're managing unique_ptrs
    CommandQueue(const CommandQueue &) = delete;
    CommandQueue &operator=(const CommandQueue &) = delete;

    // Add a command to the queue with conflict resolution
    void EnqueueCommand(std::unique_ptr<Command> command);

    bool HandleConflict(std::unique_ptr<Command> &command);

    // Get the next command to execute (returns nullptr if queue is empty)
    std::unique_ptr<Command> DequeueCommand();

    // Check if the queue has commands
    bool HasCommands() const { return !m_commands.empty(); }

    // Get current queue size
    size_t GetSize() const { return m_commands.size(); }

    // Set the conflict resolver
    void SetConflictResolver(ICommandConflictResolver *resolver) { m_conflictResolver = resolver; }

    // Clear all commands (useful for game state resets)
    void Clear();
};