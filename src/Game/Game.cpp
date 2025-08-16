#include "Game.h"
#include <iostream>

void Game::Initialize()
{
    std::cout << "Game initialized." << std::endl;
}

void Game::Update()
{
    std::cout << "Updating game logic..." << std::endl;
}

void Game::Render()
{
    std::cout << "Hello World from the Game!" << std::endl;
    Close(); // Close the game after rendering once for demonstration
}

void Game::Shutdown()
{
    std::cout << "Game shutting down." << std::endl;
}