# Future Considerations & Deferred Decisions

This file tracks architectural decisions where we deliberately chose a simpler approach now, with a known better path forward. It also captures ideas and improvements that came up during development but aren't worth acting on yet.

**When to add here:** Any time we say "this is fine for now, but later we should..." — write it down.  
**When to review:** Before planning a new feature or phase, scan this list. The right time to address a deferred item is when new work naturally touches the same area.

---

## Architecture

### Static subsystems → Service Locator or World Context

**Current:** Renderer2D, Keyboard, Mouse, and ResourceManager are all static classes with `Initialize()`/`Shutdown()` lifecycles managed by Application.  
**Concern:** Each new subsystem adds another static class. At ~8-10 subsystems, initialization order, shutdown order, and inter-dependencies become hard to reason about.  
**Future:** Consolidate into a service locator, a world/context object, or an ECS world that owns all systems. Engines like Godot use singletons with an explicit registry; Unreal uses a `UWorld` that owns subsystems.  
**When:** When the static subsystem count starts causing pain — probably around the time we add audio, physics, or networking.

### Immediate-mode OpenGL → Modern pipeline

**Current:** All rendering uses `glBegin`/`glEnd` (OpenGL 1.1 immediate mode). No vertex buffers, no shaders.  
**Concern:** Immediate mode is slow and won't support 3D, lighting, or post-processing.  
**Future:** Move to a modern OpenGL pipeline (VAO/VBO, shaders, batched rendering). This is a large change — essentially a full renderer rewrite.  
**When:** When we start a 3D project or need real rendering performance. The current approach is fine for 2D learning projects.

### Entity model → ECS

**Current:** Classical OOP entity hierarchy (Entity → GridEntity → ChessPiece → Pawn). Entities own their data and behavior.  
**Concern:** Deep inheritance hierarchies become brittle. Adding cross-cutting features (e.g., "anything with health can take damage") requires multiple inheritance or awkward mixins.  
**Future:** Migrate to an Entity-Component-System where entities are IDs, components are plain data, and systems operate on component queries. The `Sprite` class is designed as a stepping stone toward a `SpriteComponent`.  
**When:** When the current model starts causing real friction — likely when a game needs many entity types sharing partial behaviors.

### Entity lifecycle events

**Current:** Entities have `Initialize()`, `Update()`, `Render()`, `Destroy()`. There are no events for `OnCreated`, `OnDestroyed`, `OnEnabled`, etc.  
**Concern:** Some systems may want to react to entity lifecycle changes (e.g., a spatial index updating when an entity moves).  
**Future:** Add lifecycle events using the existing `Event<>` system.  
**When:** When something actually needs to observe entity lifecycle changes. Not worth adding speculatively.

---

## Build System

### `GLOB_RECURSE` for sources

**Current:** CMake uses `file(GLOB_RECURSE ...)` to collect source files automatically.  
**Concern:** CMake documentation recommends against globbing because new files aren't detected until you re-run CMake. We mitigate this by using `--fresh` in our configure tasks.  
**Future:** If this causes missed-file bugs, switch to explicit source lists or use a file-watcher wrapper.  
**When:** If someone adds a file and the build silently ignores it.

---

## Resource System

### No formal pre-caching

**Current:** `ResourceManager::Load<T>()` loads on first access and caches. There's no way to pre-load resources before they're needed (e.g., during a loading screen).  
**Future:** Add a `Preload()` or batch-load API that populates the cache upfront.  
**When:** When load hitches become noticeable (unlikely for small 2D games, more relevant with large textures or audio).

---

## Text Rendering

### Grid font → BMFont (proportional text)

**Current:** `BitmapFont` uses a fixed-width grid atlas — each character occupies the same cell size. Monospace only.  
**Concern:** Monospace looks fine for labels and scores, but proportional fonts look better for UI text, dialogue, or menus.  
**Future:** Support the BMFont `.fnt` format — a text file describing per-glyph metrics (position, offset, advance, kerning). Tools like BMFont and Hiero generate these from any TTF. The rendering loop is nearly identical; it just reads per-character advance/offset from the descriptor instead of assuming fixed width.  
**When:** When a game needs proportional text or multiple font sizes.

### BMFont → Runtime TTF / SDF

**Current (future):** BMFont atlas baked offline at a fixed size.  
**Concern:** Scaling a bitmap font causes blurriness. Multiple sizes require multiple atlases.  
**Future:** Use runtime TTF rasterization (stb_truetype or FreeType) to generate atlases at any size, or SDF/MSDF rendering for resolution-independent text (requires fragment shaders).  
**When:** When we have a modern shader pipeline and need crisp text at arbitrary sizes (likely the 3D project).

---

## Serialization

### Per-entity properties → Full scene serialization

**Current:** Entities are instantiated from `EntityInfo` structs (type name + property map) defined in code. Properties are set via the TypeRegistry's `std::any`-based setters. There is no file format, no hierarchy, and no save/load API.  
**Concern:** All "data" still lives in C++ constants (e.g. `CHESS_PIECES_DATA`). Adding a new entity means recompiling. Parent–child relationships and cross-references (e.g. "this entity points at that entity") aren't handled.  
**Future:** A hierarchical scene serialization system — load/save an entire scene to a file (JSON, YAML, or custom binary). Entities reference each other by ID. The TypeRegistry provides the reflection needed to serialize any registered property. Editor tooling can produce scene files.  
**When:** When we want a level editor, runtime scene loading, or game saves.

---

## Resource System

### Asset paths are relative to the executable

**Current:** CMake copies game assets next to the exe via `POST_BUILD`. Games use paths like `"assets/Pawn.png"`.  
**Concern:** This assumes the working directory is the exe's directory. Running from a different CWD would break.  
**Future:** Resolve asset paths relative to the executable's location (using platform APIs like `GetModuleFileName` on Windows), or introduce a virtual file system.  
**When:** If anyone runs a game from a non-standard working directory, or if we support multiple platforms.
