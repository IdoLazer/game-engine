#pragma once

#include "Application.h"

// Main entry point - this gets included by the game
int main()
{
    // Let the game create its application instance
    Engine::Application *app = Engine::CreateApplication();

    if (!app)
    {
        return -1;
    }

    // Run the application (engine takes over from here)
    app->Run();

    // Clean up
    delete app;

    return 0;
}
