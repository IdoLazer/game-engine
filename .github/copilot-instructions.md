# Game Engine Project - Copilot Instructions

## Project Overview

This is a **learning-focused C++ game engine project** designed as an educational journey into game engine architecture and development. The primary goal is understanding and learning, not commercial viability.

### Architecture

The project follows a layered architecture with clear separation of concerns and professional entry point management:

```text
src/
├── Engine/                     # Core engine systems
│   ├── Engine.h               # Single header API - everything a game needs
│   ├── Core/                  # Application lifecycle, entry point management
│   │   ├── Application.h      # Base application class + CreateApplication() declaration
│   │   └── EntryPoint.h       # Engine-owned main() function
│   ├── Entity/                # Entity component system
│   │   ├── Entity.h           # Base entity class for game objects
│   │   ├── GridEntity.h       # Grid-based entity with coordinate management
│   │   └── GridTile.h         # Simple renderable grid tile
│   ├── World/                 # World and spatial systems
│   │   └── Grid.h             # Grid coordinate system for spatial organization
│   ├── Utilities/             # General-purpose utilities
│   │   └── Timer.h            # Game timing and callback utility
│   ├── Input/                 # Input handling (Keyboard, etc.)
│   ├── Math/                  # Mathematical utilities (Vec2, etc.)
│   ├── Graphics/              # Graphics primitives (Color, etc.)
│   └── Rendering/             # Window management, rendering
└── Game/                      # Game-specific implementation
    ├── Game.h                 # Game class inheriting from Engine::Application
    ├── Game.cpp               # Game implementation + CreateApplication() factory
    └── Player.h               # Game-specific player class (Snake-specific)
```

**Key Architectural Principles:**

- **Engine-Owned Entry Point:** Engine provides `main()` via `EntryPoint.h`, games implement factory function
- **Single Header API:** Games only need `#include <Engine.h>` for complete engine access
- **Factory Pattern:** Games implement `Engine::CreateApplication()` to provide their application instance
- **Professional Include Style:** System headers (`<Engine.h>`) vs local headers (`"Game.h"`)
- **Clear Separation:** Engine infrastructure separate from game logic
- **Component-based design:** Small, focused classes with clear responsibilities
- **CMake build system:** vcpkg for dependency management, system includes for engine headers
- **Modular Engine Organization:** Entity system, World systems, Utilities clearly separated
- **Generic vs Game-Specific:** Reusable classes in Engine, specific implementations in Game

### Current Technology Stack

- **Language:** C++17
- **Build System:** CMake with vcpkg
- **Graphics:** OpenGL + GLFW
- **Platform:** Windows (primary), designed for cross-platform compatibility

## Development Guidelines

### Code Quality & Style

- **Incremental Development:** Add features in small, testable chunks
- **Clarity over Cleverness:** Write clear, readable code that's easy to understand
- **Small Components:** Prefer many small, focused classes over monolithic ones
- **Proper Namespacing:** Engine code lives in `Engine` namespace
- **RAII Principles:** Proper resource management with constructors/destructors

### Learning Partnership Approach

**For the Developer:**

- This is a learning project - mistakes and questions are expected and welcome
- The developer has self-described "weak" C++ skills - provide educational explanations
- Always explain the "why" behind design decisions
- Walk through code step-by-step, don't dump large blocks

**For Copilot (AI Assistant):**

- **Be a Learning Partner:** Challenge approaches constructively
- **Don't Assume Correctness:** Question design decisions if there are better alternatives
- **Explain Everything:** Walk through what you're doing and why
- **Incremental Changes:** Never write large amounts of code at once
- **Present Options:** When multiple approaches exist, explain pros/cons and ask for preference
- **Stop and Ask:** If you encounter issues, present options rather than trying multiple solutions
- **Educational Focus:** Teaching moments are more important than just getting things working
- **Reference Sources:** When using patterns or techniques, reference learning materials or documentation (use the references provided in the project under references/)

### Implementation Workflow

1. **Understand the Request:** Make sure you understand what the developer wants to achieve
2. **Architectural Review:** Consider how the change fits into the current architecture
3. **Alternative Analysis:** If there are better approaches, discuss them before implementing
4. **Small Steps:** Break changes into small, testable increments
5. **Explain Each Step:** Walk through what you're adding and why
6. **Test as You Go:** Ensure each increment compiles and works before moving on

### Common Patterns to Follow

- **Engine-Owned Entry Point:** Engine provides `main()`, games implement `Engine::CreateApplication()`
- **Single Header Include:** Games use `#include <Engine.h>` for all engine functionality
- **Factory Pattern:** Games return their application instance via the CreateApplication() function
- **Inheritance for Core Systems:** Game inherits from Engine::Application
- **Abstract Interfaces:** Engine provides pure virtual methods for game implementation
- **Professional Include Style:** `<Engine.h>` for engine, `"Game.h"` for local files
- **Resource Management:** Use RAII, smart pointers where appropriate
- **Error Handling:** Clear error messages and graceful degradation
- **Configuration:** Use config structs for system setup (like WindowConfig)

### What NOT to Do

- Don't implement multiple large features simultaneously
- Don't use overly complex C++ features without explanation
- Don't assume the developer understands advanced patterns
- Don't proceed with major architectural changes without discussion
- Don't optimize prematurely - clarity first, performance later

### Current Development Status

The engine currently has:

- ✅ Engine-owned entry point architecture (professional main() handling)
- ✅ Single header API (`#include <Engine.h>`)
- ✅ Factory pattern for application creation
- ✅ Basic application lifecycle (Application class)
- ✅ Window management with GLFW
- ✅ Basic input handling (Keyboard)
- ✅ Clean Game/Engine separation
- ✅ CMake build system with vcpkg
- ✅ Professional include style (system vs local headers)
- ✅ Entity system (Entity, GridEntity, GridTile)
- ✅ World systems (Grid coordinate system)
- ✅ Utilities (Timer for game timing)
- ✅ Modular Engine organization

Next logical areas for development:

- Rendering pipeline improvements
- Resource management
- Audio system
- Physics integration
- Scene management
- Enhanced Entity-Component System features

### Questions to Ask Yourself Before Making Changes

1. Does this change maintain the learning-focused approach?
2. Is this the simplest way to achieve the goal?
3. Can I explain this clearly to someone learning C++?
4. Are there alternative approaches we should discuss?
5. Is this change small enough to understand and test easily?

Remember: The journey is more important than the destination. Every change should be an opportunity to learn something new about game engine architecture, C++, or software design.
