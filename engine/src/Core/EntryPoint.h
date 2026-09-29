#pragma once

#include "Application.h"
#include "../Resources/ResourceManager.h"

// Main entry point - this gets included by the game
int main()
{
    // GAME_SOURCE_DIR is defined for Debug builds only, and this header is compiled into the game
    // target, so the engine can read assets from the source tree without the game knowing.
#ifdef GAME_SOURCE_DIR
    Engine::ResourceManager::SetAssetRoot(GAME_SOURCE_DIR);
#endif

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
