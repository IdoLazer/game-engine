# Simple Game Engine

A barebones C++ project with clean separation between Engine utilities and Game logic.

## Project Structure

GameEngine/
├── Engine/                 # Engine utilities and core functionality
│   └── ...
├── Game/                   # Your game-specific code
│   └── ...
├── main.cpp                # Entry point
├── Makefile                # Build configuration
└── build/                  # Compiled files (created when building)

## What's What

- **Engine/**: Contains utility functions like logging, initialization, etc. This is where you'd add things like window management, graphics, input handling as you expand.
- **Game/**: Your actual game code that uses the engine utilities. This is where your game logic lives.
- **main.cpp**: Simple entry point that creates and runs your game.

## Building and Running

```bash
# Build and run
cmake --preset=default
cmake --build build
```
