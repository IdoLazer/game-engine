# Simple Game Engine

A learning-focused C++ game engine built with OpenGL and GLFW, featuring engine-owned entry points, a type registry with runtime instantiation, and a grid-based entity system.

## Project Structure

```text
engine/
└── src/                                # Core engine systems
    ├── Engine.h                        # Single header API — everything a game needs
    ├── Core/                           # Application lifecycle, entry point, scene management
    │   ├── Application.h               # Base application class + CreateApplication() declaration
    │   ├── EntryPoint.h                # Engine-owned main() function
    │   └── Scene.h                     # Entity ownership and lifecycle management
    ├── Entity/                         # Entity system
    │   ├── Entity.h                    # Base entity class (with Scene access for spawning)
    │   ├── GridEntity.h                # Grid-based entity with coordinate management
    │   ├── GridTile.h                  # Simple renderable grid tile
    │   └── TextEntity.h                # Text label entity using BitmapFont
    ├── Events/                         # Observer pattern event system
    │   ├── Event.h                     # Event<Args...> / EventSubscriber<Args...>
    │   └── Subscription.h              # RAII subscription handle
    ├── Types/                          # Runtime type registry and self-registration
    │   ├── TypeRegistry.h              # Singleton mapping type names → factories + properties
    │   └── TypeRegistrationMacros.h    # DECLARE_TYPE / REGISTER_PROPERTY macros
    ├── Patterns/                       # Reusable design patterns
    │   └── Command/                    # Command pattern (Command, CommandQueue)
    ├── World/                          # Spatial systems
    │   ├── Grid.h                      # Spatial index over grid cells
    │   ├── GridCoordinateSystem.h      # Pure coordinate math (grid ↔ world)
    │   └── TileMap.h                   # Abstract tile map with per-cell rendering
    ├── Input/                          # Keyboard and Mouse (static API)
    ├── Math/                           # Vec2
    ├── Graphics/                       # Color
    ├── Rendering/                      # Window, Renderer2D, Camera2D, Texture2D, Sprite, BitmapFont
    ├── Resources/                      # Resource base class, ResourceManager (static API)
    └── Utilities/                      # Timer
games/
├── Snake/                              # Snake game
│   ├── src/
│   │   ├── Snake.h/cpp                 # Application subclass + CreateApplication()
│   │   ├── SnakeConstants.h            # Tuning constants
│   │   ├── Player.h/cpp               # Snake player (GridEntity + movement + tail)
│   │   └── Commands/                   # Input → Command pattern wiring
│   └── assets/                         # Game-specific assets (copied to build dir)
└── Chess/                              # Chess game
    ├── src/
    │   ├── Chess.h/cpp                 # Application subclass + CreateApplication()
    │   ├── ChessBoard.h/cpp            # Board with tile and piece management
    │   ├── ChessTile.h/cpp             # Clickable board tile
    │   └── Pieces/                     # Piece hierarchy (Pawn, Rook, Bishop, Knight, Queen, King)
    └── assets/                         # Game-specific assets (copied to build dir)
tests/                                  # Google Test suite
├── Vec2Test.cpp                        # Vec2 math tests
├── EventTest.cpp                       # Event system tests
└── GridCoordinateSystemTest.cpp        # Grid coordinate system tests
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
    void Render() override;
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

## Resource System

`ResourceManager` is a static subsystem that loads, caches, and owns assets:

```cpp
// Load a texture — cached on subsequent calls, shared across all users
auto* texture = ResourceManager::Load<Texture2D>("assets/Pawn.png");
```

All paths are resolved relative to the executable's directory. CMake copies each game's `assets/` folder next to its exe at build time via a `POST_BUILD` step, so games use clean relative paths regardless of working directory.

Loadable resource types inherit from `Resource`. Currently `Texture2D` and `BitmapFont` exist; future types (audio, etc.) follow the same pattern.

## Text Rendering

`BitmapFont` is a `Resource` that loads a grid-based character atlas. `TextEntity` renders text as an entity in the scene:

```cpp
// Load a bitmap font — cached like any other resource
auto* font = ResourceManager::Load<BitmapFont>("assets/font.png");

// Create a text label as a scene entity
auto* label = scene->Instantiate<TextEntity>();
label->SetFont(font);
label->SetText("Hello!");
label->SetCharHeight(0.5f);
label->SetColor(Color::White);
label->SetWorldPosition(Vec2(-3.0f, 4.0f));
```

The font atlas is a PNG with characters in a 16-column × 6-row grid (ASCII 32–127). `Renderer2D::DrawText()` is also available for direct draw calls.

## Entity System

All game objects inherit from `Entity`. The engine provides a hierarchy for grid-based games:

- **`Entity`** — base class with world position, size, color, and `Scene` access
- **`TextEntity`** — renders a text string using a `BitmapFont`
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

## Testing

Tests use [Google Test](https://google.github.io/googletest/) (fetched automatically via CMake FetchContent). Test files live in `tests/` and are compiled into a single `Tests` executable.

```bash
# Build and run tests
cmake --build build/windows-x64-debug --target Tests
ctest --test-dir build/windows-x64-debug --output-on-failure

# Filter by name
ctest --test-dir build/windows-x64-debug -R "Vec2"
```

VS Code tasks are also available: **Build + Test (Debug)** builds and runs all tests via CTest.

## Technology

- **C++20** with modern language features
- **CMake** with vcpkg for dependency management
- **OpenGL + GLFW** for rendering and windowing
- **stb_image** for PNG/image loading (via vcpkg)
- **Windows** (primary), designed for cross-platform compatibility
