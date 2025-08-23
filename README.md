# Simple Game Engine

A learning-focused C++ game engine with a clean, professional architecture featuring engine-owned entry points and elegant API design.

## Project Structure

```text
src/
├── Engine/                     # Core engine systems
│   ├── Engine.h               # Single header - everything a game needs!
│   ├── Core/                  # Application lifecycle, entry point
│   │   ├── Application.h      # Base application class
│   │   └── EntryPoint.h       # Engine-owned main() function
│   ├── Input/                 # Input handling (Keyboard, etc.)
│   └── Rendering/             # Window management, rendering
└── Game/                      # Game-specific implementation
    ├── Game.h                 # Game class inheriting from Engine::Application
    └── Game.cpp               # Game implementation + CreateApplication() factory
```

## Architecture Highlights

- **Engine-Owned Entry Point:** The engine provides `main()` - games just implement a factory function
- **Single Header API:** Games only need `#include <Engine.h>`
- **Professional Design:** Clean separation between engine infrastructure and game logic
- **Factory Pattern:** Games implement `Engine::CreateApplication()` to provide their application instance

## How to Create a Game

Creating a game with this engine is simple:

```cpp
// Game.h
#include <Engine.h>

class MyGame : public Engine::Application {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Shutdown() override;
};

// Game.cpp  
#include "Game.h"

// Implement your game methods...

// Factory function - this is how the engine gets your game
Engine::Application* Engine::CreateApplication() {
    return new MyGame();
}
```

That's it! The engine handles:

- Entry point (`main()` function)
- Engine initialization and shutdown
- Main game loop
- Window management
- Input handling
- Rendering setup

## Building and Running

```bash
# Configure and build
cmake --preset=default
cmake --build build --config Debug

# Run the game
.\build\Game.exe
```
