#include "Game/Game.h"
#include <iostream>

int main()
{
    std::cout << "Starting Game..." << std::endl;

    // Create the game instance
    Game *game = new Game();

    // Run the game
    game->Run();

    // Clean up
    delete game;

    std::cout << "Game finished." << std::endl;
    return 0;
}