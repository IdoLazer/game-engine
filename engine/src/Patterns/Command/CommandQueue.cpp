#include "CommandQueue.h"

namespace Engine
{
    // --- Queue Operations ---

    void CommandQueue::EnqueueCommand(std::unique_ptr<Command> command)
    {
        if (!command)
        {
            return; // Don't enqueue null commands
        }

        if (m_conflictResolver && HandleConflict(command))
        {
            return; // Conflict handled, don't enqueue the command
        }

        // If queue is at maximum capacity, we need to handle overflow
        if (m_commands.size() >= MAX_QUEUE_SIZE)
        {
            m_commands.pop();
        }
        m_commands.push(std::move(command));
    }

    // --- Internal ---

    bool CommandQueue::HandleConflict(std::unique_ptr<Command> &command)
    {
        // Check if the new command is conflicting with any queued command
        std::queue<std::unique_ptr<Command>> tempQueue;
        bool foundConflict = false;

        // Check each command in the queue
        while (!m_commands.empty())
        {
            auto queuedCmd = std::move(m_commands.front());
            m_commands.pop();

            // If this queued command is conflicting with our new command, replace it
            if (m_conflictResolver->IsConflict(command.get(), queuedCmd.get()))
            {
                tempQueue.push(std::move(command)); // Replace with new command
                foundConflict = true;
                break; // Once we replaced a command, we drop the remaining queued commands
            }
            else
            {
                tempQueue.push(std::move(queuedCmd)); // Keep the original command
            }
        }

        // Move commands back to main queue
        m_commands = std::move(tempQueue);

        return foundConflict;
    }

    std::unique_ptr<Command> CommandQueue::DequeueCommand()
    {
        if (m_commands.empty())
        {
            return nullptr;
        }

        auto command = std::move(m_commands.front());
        m_commands.pop();
        return command;
    }

    void CommandQueue::Clear()
    {
        while (!m_commands.empty())
        {
            m_commands.pop();
        }
    }

    // --- Configuration ---

    void CommandQueue::SetConflictResolver(std::unique_ptr<ICommandConflictResolver> resolver)
    {
        m_conflictResolver = std::move(resolver);
    }

    // --- Accessors ---

    bool CommandQueue::HasCommands() const { return !m_commands.empty(); }
    size_t CommandQueue::GetSize() const { return m_commands.size(); }
}