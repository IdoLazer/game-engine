# Simple Game Engine

A barebones C++ project with clean separation between Engine utilities and Game logic.

## Project Structure

GameEngine/
├── Engine/                 # Engine utilities and core functionality
│   ├── Application.h       # Application header
│   └── Application.cpp     # Application implementation
├── Game/                   # Your game-specific code
│   ├── Game.h              # Game header
│   └── Game.cpp            # Game implementation
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
make run

# Just build
make

# Clean build files
make clean
```

## Next Steps

As you expand, you can add to the Engine:

- Window creation and management
- Graphics rendering
- Input handling
- Audio
- File I/O utilities
- Math utilities

And to the Game:

- Game states
- Player logic
- World/level management
- Game-specific systems
