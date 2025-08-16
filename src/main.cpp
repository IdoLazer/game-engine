#include "Game/Game.h"

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