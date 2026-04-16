# Simple Game Engine

A learning-focused C++ game engine built with OpenGL and GLFW, featuring engine-owned entry points, a type registry with runtime instantiation, and a grid-based entity system.

## Project Structure

```text
src/
├── Engine/                             # Core engine systems
│   ├── Engine.h                        # Single header API — everything a game needs
│   ├── Core/                           # Application lifecycle, entry point, scene management
│   │   ├── Application.h               # Base application class + CreateApplication() declaration
│   │   ├── EntryPoint.h                # Engine-owned main() function
│   │   └── Scene.h                     # Entity ownership and lifecycle management
│   ├── Entity/                         # Entity system
│   │   ├── Entity.h                    # Base entity class (with Scene access for spawning)
│   │   ├── GridEntity.h                # Grid-based entity with coordinate management
│   │   └── GridTile.h                  # Simple renderable grid tile
│   ├── Types/                          # Runtime type registry and self-registration
│   │   ├── TypeRegistry.h              # Singleton mapping type names → factories + properties
│   │   └── TypeRegistrationMacros.h    # DECLARE_TYPE / REGISTER_PROPERTY macros
│   ├── Patterns/                       # Reusable design patterns
│   │   └── Command/                    # Command pattern (Command, CommandQueue)
│   ├── World/                          # Spatial systems
│   │   ├── Grid.h                      # Spatial index over grid cells
│   │   ├── GridCoordinateSystem.h      # Pure coordinate math (grid ↔ world)
│   │   └── TileMap.h                   # Abstract tile map with per-cell rendering
│   ├── Input/                          # Keyboard and Mouse (static API)
│   ├── Math/                           # Vec2
│   ├── Graphics/                       # Color
│   ├── Rendering/                      # Window, Renderer2D, Camera2D
│   └── Utilities/                      # Timer
├── Snake/                              # Snake game
│   ├── Snake.h/cpp                     # Application subclass + CreateApplication()
│   ├── SnakeConstants.h                # Tuning constants
│   ├── Player.h/cpp                    # Snake player (GridEntity + movement + tail)
│   └── Commands/                       # Input → Command pattern wiring
└── Chess/                              # Chess game
    ├── Chess.h/cpp                     # Application subclass + CreateApplication()
    ├── ChessBoard.h/cpp                # Board with tile and piece management
    ├── ChessTile.h/cpp                 # Clickable board tile
    └── Pieces/                         # Piece hierarchy (Pawn, Rook, Bishop, Knight, Queen, King)
```

## How It Works

The engine owns `main()`. A game implements a factory function and the engine handles everything else:

```cpp
// Snake.h
#include <Engine.h>
#include <Core/EntryPoint.h>   // injects main()

class Snake : public Engine::Application {
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render() const override;
    void Shutdown() override;
};

// Snake.cpp
#include "Snake.h"

void Snake::Initialize() { /* set up world, entities, etc. */ }
void Snake::Update(float deltaTime) { /* game logic */ }
void Snake::Render() const { /* draw calls */ }
void Snake::Shutdown() { /* cleanup */ }

namespace Engine {
    Application* CreateApplication() { return new Snake(); }
}
```

The engine's `Application` base class provides:

- Window creation and management via `GetWindowConfig()` override
- A `Scene` for entity ownership (`GetScene()->Instantiate<T>()`)
- A main loop that calls `Initialize → Update/Render → Shutdown`
- `Close()` to request a clean exit

## Entity System

All game objects inherit from `Entity`. The engine provides a hierarchy for grid-based games:

- **`Entity`** — base class with world position, size, color, and `Scene` access
- **`GridEntity`** — adds grid coordinates that auto-sync to world coordinates, registers with a spatial `Grid`
- **`GridTile`** — a simple filled rectangle at a grid position

Entities are created through the `Scene`:

```cpp
auto* player = GetScene()->Instantiate<Player>();   // compile-time type
auto* entity = GetScene()->Instantiate("GridTile");  // runtime type name
```

## Type Registry

Entity types self-register at static initialization time using macros:

```cpp
// Header
class Player : public Engine::GridEntity {
    DECLARE_TYPE(Player, GridEntity)
    // ...
};

// Source
BEGIN_TYPE_REGISTER(Player)
    REGISTER_PROPERTY(Player, m_moveSpeed, "MoveSpeed")
END_TYPE_REGISTER(Player)
```

This enables runtime instantiation by name and data-driven property assignment — the foundation for future scene serialization.

## Building

Requires CMake, a C++20 compiler, and vcpkg.

```bash
# Configure
cmake --preset windows-x64-debug

# Build a target
cmake --build build/windows-x64-debug --target Snake
cmake --build build/windows-x64-debug --target Chess

# Run
.\build\windows-x64-debug\bin\Snake\Snake.exe
.\build\windows-x64-debug\bin\Chess\Chess.exe
```

## Technology

- **C++20** with modern language features
- **CMake** with vcpkg for dependency management
- **OpenGL + GLFW** for rendering and windowing
- **Windows** (primary), designed for cross-platform compatibility
