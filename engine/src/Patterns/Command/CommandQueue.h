#pragma once

#include "Command.h"
#include <memory>
#include <queue>

namespace Engine
{
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
    public:
        // --- Constructors & Destructors ---
        CommandQueue() = default;
        ~CommandQueue() = default;

        // Non-copyable since we're managing unique_ptrs
        CommandQueue(const CommandQueue &) = delete;
        CommandQueue &operator=(const CommandQueue &) = delete;

        // --- Queue Operations ---
        void EnqueueCommand(std::unique_ptr<Command> command);
        std::unique_ptr<Command> DequeueCommand();
        void Clear();

        // --- Configuration ---
        void SetConflictResolver(std::unique_ptr<ICommandConflictResolver> resolver);

        // --- Accessors ---
        bool HasCommands() const;
        size_t GetSize() const;

    private:
        // --- Fields ---
        static constexpr size_t MAX_QUEUE_SIZE = 2;
        std::queue<std::unique_ptr<Command>> m_commands;
        std::unique_ptr<ICommandConflictResolver> m_conflictResolver;

        // --- Internal ---
        bool HandleConflict(std::unique_ptr<Command> &command);
    };
}