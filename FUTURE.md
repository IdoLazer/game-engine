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
**Note:** In ECS, "entity" doesn't imply a position or visual presence — it's just an ID. Manager-type objects (like `PlatformerInputManager`) could naturally become entities and get lifecycle management (`Initialize`, `Update`) for free, without the game manager manually orchestrating them. This is one of the concrete wins of ECS for non-visual systems.  
**When:** When the current model starts causing real friction — likely when a game needs many entity types sharing partial behaviors.

### Entity lifecycle events

**Current:** Entities have `Initialize()`, `Update()`, `Render()`, `Destroy()`. There are no events for `OnCreated`, `OnDestroyed`, `OnEnabled`, etc.  
**Concern:** Some systems may want to react to entity lifecycle changes (e.g., a spatial index updating when an entity moves).  
**Future:** Add lifecycle events using the existing `Event<>` system.  
**When:** When something actually needs to observe entity lifecycle changes. Not worth adding speculatively.

### `PlayerStateMachine` → `Engine::StateMachine<TStateId>`

**Current:** `Player`'s movement is driven by a hierarchical `PlayerState`/`PlayerStateMachine` in `games/Platformer/src/StateMachine/` — a concrete, non-templated implementation scoped to `Player`. It handles parent/child states, root-to-leaf `Update`, child-to-root input bubbling, and shared-ancestor transitions.
**Concern:** Falcon (`FalconState` + a plain `enum`+`switch` in `Falcon::Update`) has the same shape of problem and would benefit from the same pattern, but the machine is hardcoded to `PlayerState`/`PlayerStateId` and can't be reused as-is. The notification methods (`JumpPressed`, `ContactsResolved`, `AdjustVisual`) are Player-specific by design — a generic version needs a way to keep that expressiveness without turning into a stringly-typed event bus.
**Future:** Promote the transition/dispatch machinery to `Engine::State`/`Engine::StateMachine<TStateId>` in `engine/src/Patterns/State/`, mirroring the existing `Command`/`CommandQueue` pattern, and let games subclass it to add their own typed notifications. Migrate Player, then optionally Falcon. Add a `StateMachineTest.cpp` covering shared-ancestor transition ordering (parent stays entered between siblings), reentrant `TransitionTo` from within `Enter()`, and input bubbling.
**When:** When Falcon's states need a rework anyway, or when a third state machine appears. Not before — the second use case is what will show which parts are genuinely general.

### Player physics primitives are still reachable from any state

**Current:** The movement states are nested classes of `Player`, so each one can touch every private member of the body — velocity, config coefficients, the wall jump history.
**Concern:** Nesting is what keeps `Player`'s API narrow (nothing had to become public for the states to work), but it also means a state *could* reach for something it has no business touching. Discipline, not the compiler, is what keeps `GlidingState` out of the jump buffer.
**Future:** If it ever bites, give the states a narrow `PlayerBody` interface holding just the primitives, and pass that to `PlayerState` instead of `Player &`.
**When:** Only if a state actually reaches somewhere it shouldn't. Not worth the indirection speculatively.

### Pre-initialization hook (e.g. `Awake()` or `OnCreated()`)

**Current:** `Initialize()` is deferred — it runs during `FlushPending()` on the first `Update()` frame, not immediately after `Instantiate()`. There is no hook that runs right after an entity is created and its properties are set.  
**Concern:** Code that needs to set up an entity before the first frame (e.g., positioning the player at a spawn point from the world grid) must rely on initialization order between entities within `FlushPending()`. This works today because entities initialize in instantiation order, but it's implicit and fragile.  
**Future:** Add an earlier lifecycle hook (similar to Unity's `Awake()`) that runs immediately after `Instantiate()` + property assignment, before the first frame. `Initialize()` would then only contain logic that's safe to depend on all other entities being awake.  
**When:** When the implicit initialization order causes bugs or when cross-entity setup becomes too complex to reason about.

---

## Collision

### Non-tile static colliders

**Current:** `PlatformerWorld`'s collision broad phase (`SweepSolid`/`RaycastSolid`/`OverlapsSolid`) only enumerates candidate `Rect`s derived from the tile grid.  
**Concern:** A hand-placed `Rect` that isn't cell-aligned or cell-sized (e.g. a narrow platform or a switch) has no way to participate in Player/Falcon collision sweeps today.  
**Future:** Extend `PlatformerWorld` (or a small sibling holding `std::vector<Rect>`) with manually-placed static colliders, and extend the candidate-gathering step in `SweepSolid`/`RaycastSolid`/`OverlapsSolid` to test both sources. The narrow-phase math (`Rect`, `SweepRectVsRect`) already supports this with zero changes - only broad-phase candidate-gathering needs extending.  
**When:** When the first non-tile collider entity is actually designed.

### Falcon flight collision

**Current:** Neither flight collides per-frame. `Falcon::ReturnToPlayer` flies straight back to the player with no check at all. `MoveToGoal` (outbound) flies straight to `m_latchPoint`, a point validated once by `SetGoal`'s aim-time raycast - not re-checked while flying, so it can't stop partway on incidental geometry a per-frame sweep would catch.  
**Concern:** Inconsistent with how solid the rest of the world behaves - nothing stops the falcon mid-flight in either direction, only the outbound aim itself is validated.  
**Future:** If flying through walls (return trip) or through geometry that appeared after aiming (outbound) actually causes a problem, give both flights a per-frame check - once there's a clear answer for what it should do when blocked (stop and wait, slide along the obstacle, something else).  
**When:** When flying through walls on retrieval, or through changed geometry mid-flight outbound, actually causes a problem in practice, or when we want a consistently physical world.

---

## Falcon Gameplay Systems

Four traversal mechanics tied to the Falcon's state, designed together so each one expresses "what does the falcon's body do from this angle" rather than an arbitrary elemental effect per direction. None of these are implemented yet - logged here before starting so the reasoning behind their shape isn't lost.

### Shoulder glide
**Concept:** While the falcon rests on the player's shoulder (`FalconState::OnShoulder`), the player can hold onto its legs mid-air and glide instead of free-falling after a jump.  
**Why:** The most literal reading of "boy has a magical falcon companion" - carrying it turns a fall into flight.

### Stoop dash
**Concept:** While gliding, the player can aim the falcon (the same aim used for latching) and trigger a fast dash towards the aim point, ending the glide.  
**Why:** Riffs on the peregrine falcon's stoop - the fastest dive in nature - giving the glide state a second, more aggressive option instead of only ever being a slow-fall.

### Perch platform
**Concept:** When the falcon is latched into a wall (`m_latchDirection` horizontal), its body becomes a small horizontal platform the player can stand on.  
**Why:** A perched falcon is literally a perch - furniture, not magic.

### Rope swing
**Concept:** When the falcon is latched into a ceiling (`m_latchDirection` pointing up), the player can grab on below it and swing like a pendulum to cross gaps.  
**Why:** A falcon hanging from a ceiling reads naturally as something to swing from.

### Updraft dash
**Concept:** When the falcon is latched into the floor (`m_latchDirection` pointing down), it flaps its wings to give a nearby player an upward dash of momentum - an area effect near the falcon rather than a stand-on-it platform, so it's usable off a run-up instead of requiring a precise landing on its body.  
**Why:** Reframed from an earlier "magic air vortex" idea into a physical wing-flap, so all four latch states stay in the same "physical consequence of the falcon's pose" register instead of one of them being an arbitrary elemental effect.

**Open questions for implementation:** exact glide/dash speed and duration tuning; how the updraft's trigger area is detected (radius check? a `Rect` near the falcon?); whether Perch/Rope/Updraft need real collision registration in `PlatformerWorld` (see "Non-tile static colliders" above) or can be handled bespoke in `Player`/`Falcon` since they're tied to a single entity rather than level geometry.

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

### Asset paths are relative to the executable

**Current:** CMake copies game assets next to the exe via `POST_BUILD`. Games use paths like `"assets/Pawn.png"`.  
**Concern:** This assumes the working directory is the exe's directory. Running from a different CWD would break.  
**Future:** Resolve asset paths relative to the executable's location (using platform APIs like `GetModuleFileName` on Windows), or introduce a virtual file system.  
**When:** If anyone runs a game from a non-standard working directory, or if we support multiple platforms.

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

### File-based level data

**Current:** Level grids (tile layouts) are defined as C++ constants in game code. Adding or changing a level requires recompilation.  
**Concern:** Designers can't iterate on levels without a C++ toolchain. The data-in-code pattern won't scale to dozens of levels.  
**Future:** Load level data from files (JSON, CSV, or a custom text format). This requires choosing a format, adding a file-parsing module to the engine, and integrating with the resource system. Could pair with an editor that exports levels directly.  
**When:** When level count grows beyond what's comfortable in code, or when we build editor tooling.
