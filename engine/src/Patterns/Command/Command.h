#pragma once

namespace Engine
{
    // Base Command interface for the Command pattern
    // This allows us to encapsulate requests as objects, enabling:
    // - Queuing of commands
    // - Undo functionality (future enhancement)
    // - Parameterization of objects with different requests

    class Command
    {
    public:
        virtual ~Command() = default;

        // Execute the command
        virtual void Execute() = 0;

        // Optional: Get a description for debugging
        virtual const char *GetName() const = 0;
    };
}