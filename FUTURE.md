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

### Game-local `State`/`StateMachine` → `Engine::StateMachine`

**Current:** `State<TStateId>` and `StateMachine<TStateId, TState>` in `games/Platformer/src/StateMachine/` are generic and vocabulary-free, but still game-local. `Player` and `Falcon` both run on them.
**Concern:** Being game-local, it's untested — `tests/` covers engine modules only — so playing the game is the only thing exercising it. Promoting it makes it public API for Snake and Chess too.
**Future:** Move the two headers to `engine/src/Patterns/State/`, wrap them in `namespace Engine`, and add them to `Engine.h`. Drop `GetActiveChain` if it still has no callers by then. Add a `tests/StateMachineTest.cpp` covering shared-ancestor transition ordering (parent stays entered between siblings), reentrant `TransitionTo` from within `Enter()`, and input bubbling.
**When:** Ready whenever — two use cases have held the shape. The open question is only whether to wait for a third, non-Platformer one.

### An entity's primitives are still reachable from any of its states

**Current:** The states are nested classes of the entity they drive — `Player`'s movement states can touch velocity, config coefficients and the wall jump history; `Falcon`'s behavior states can touch the aim point, latch point and command queue. `FalconState`, the shared base, isn't nested and reaches the same primitives through an explicit `friend`.
**Concern:** Nesting is what keeps each entity's API narrow (nothing had to become public for the states to work), but it also means a state _could_ reach for something it has no business touching. Discipline, not the compiler, is what keeps `GlidingState` out of the jump buffer.
**Future:** If it ever bites, give the states a narrow body interface holding just the primitives (`PlayerBody`, `FalconBody`), and pass that to the state base instead of `Player &`/`Falcon &`. That would also retire the `friend`.
**When:** Only if a state actually reaches somewhere it shouldn't. Not worth the indirection speculatively.

### Pre-initialization hook (e.g. `Awake()` or `OnCreated()`)

**Current:** `Initialize()` is deferred — it runs during `FlushPending()` on the first `Update()` frame, not immediately after `Instantiate()`. There is no hook that runs right after an entity is created and its properties are set.  
**Concern:** Code that needs to set up an entity before the first frame (e.g., positioning the player at a spawn point from the world grid) must rely on initialization order between entities within `FlushPending()`. This works today because entities initialize in instantiation order, but it's implicit and fragile.  
**Future:** Add an earlier lifecycle hook (similar to Unity's `Awake()`) that runs immediately after `Instantiate()` + property assignment, before the first frame. `Initialize()` would then only contain logic that's safe to depend on all other entities being awake.  
**When:** When the implicit initialization order causes bugs or when cross-entity setup becomes too complex to reason about.

### Input reaches entities only as presses and releases

**Current:** `PlatformerInputManager` publishes press and release events. `Player` and `Falcon` keep one-slot command queues to remember that a button is still down - a glide or an aim asked for too early, a jump released during its minimum arc - and a level load drops a held glide or aim, because the new entities never saw the press.
**Concern:** Every mechanic driven by a held button adds another queue and another place that has to clear it.
**Future:** Let the input manager also expose current state (move direction; jump, glide and aim held) for a state to read when it becomes able to act. Whether a held button should act without a fresh press is a feel decision per input, not only a refactor.
**When:** When a new held-input mechanic needs another queue, or the dropped input at a level transition is noticed in play.

### A scene and the application share one lifecycle

**Current:** `Application::ReloadScene` runs `Shutdown`, clears the scene and runs `Initialize`, so a game's `Initialize` and `Shutdown` bracket one scene, not the run. What has to outlive a scene is built in the game's constructor, as the Platformer does with its input manager, before the engine's subsystems exist.
**Concern:** Something that outlives a scene but needs the window, the renderer or a resource has nowhere to be created.
**Future:** Run `Initialize` and `Shutdown` once, and add hooks the engine calls around every scene build. Snake and Chess would move their scene setup into the new hook.
**When:** When a game needs such an object.

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

**Concept:** While the falcon rests on the player's shoulder (`FalconStateId::OnShoulder`), the player can hold onto its legs mid-air and glide instead of free-falling after a jump.  
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

## Levels

### Level size is fixed by GRID_WORLD_SIZE

**Current:** `TileGrid` reports its own row and column counts, and the document format imposes no size. But `Platformer::Initialize` still builds `m_grid` from `PlatformerConstants::GRID_WORLD_SIZE` (30x20), and the camera shows exactly that.
**Concern:** A differently sized level would parse correctly and then render and collide against the wrong grid.
**Future:** Build the `Grid` from the loaded `TileGrid`'s dimensions. That is a few lines; the open question is what the camera should do, since showing a bigger level whole means smaller cells.
**When:** When a level wants to be a size other than 30x20, which likely arrives with a scrolling camera.

### Level entities cannot reach anything that outlives the level

**Current:** A level load destroys every entity and builds the next level's. `Platformer` itself persists, but entities only see the `Scene`, so nothing they can reach survives the load.
**Concern:** An objective that changes the world - a sluice opened in one level, a conversation that differs on the way back - needs state that outlives the level it happened in, and that a death does not undo.
**Future:** A small game-state object owned by `Platformer`, handed to the entities that need it when a level is wired.
**When:** When the first objective has to be remembered across a level load.

### Only Platformer knows how to place an anchored entity

**Current:** `SpawnPoint` and `LevelExit` name an anchor, and `Platformer` looks the anchor up in the tile grid when it loads a level. The entities themselves hold no position.
**Concern:** The next level object - a platform, a control, a sign - needs the same lookup, plus a position of its own to update and draw at.
**Future:** A small base for anchored level entities that takes its bounds from the tile grid when the level is wired, so a new type only adds its behaviour.
**When:** When the first level entity that has to draw or move is added.

### An exit's target is checked only when the player reaches it

**Current:** A `LevelExit` names its target level and spawn point as text, and nothing reads them until the player enters the exit. A level path that does not load throws out of `Initialize` and ends the game; a spawn name the target lacks is reported and the player appears at the target's first `SpawnPoint`.
**Concern:** A typo in an exit is found only by playing to it, and a wrong path costs a restart.
**Future:** Check every exit's target when its level loads, and report it alongside the anchor problems.
**When:** When there are more exits than are walked through in a normal playtest.

---

## Build System

### `GLOB_RECURSE` for sources

**Current:** CMake uses `file(GLOB_RECURSE ...)` to collect source files automatically.  
**Concern:** CMake documentation recommends against globbing because new files aren't detected until you re-run CMake. We mitigate this by using `--fresh` in our configure tasks.  
**Future:** If this causes missed-file bugs, switch to explicit source lists or use a file-watcher wrapper.  
**When:** If someone adds a file and the build silently ignores it.

### Per-game compile definitions reach only one engine source

**Current:** `Engine` is an `OBJECT` library compiled once and linked into every game, so a game's compile definitions never reach it. `ResourceManager.cpp` needs one (`GAME_SOURCE_DIR`), so CMake leaves it out of the library and compiles it into each game instead.  
**Concern:** It is a special case in the build - one engine file compiled once per target - and every further per-game value adds another file to that list.  
**Future:** Hand per-game settings to the engine at runtime instead: a project file beside the executable, or a command-line argument like Quake's `-basedir`. No engine source would then depend on which game it is built for.  
**When:** When a second engine source needs a per-game value.

---

## Resource System

### No formal pre-caching

**Current:** `ResourceManager::Load<T>()` loads on first access and caches. There's no way to pre-load resources before they're needed (e.g., during a loading screen).  
**Future:** Add a `Preload()` or batch-load API that populates the cache upfront.  
**When:** When load hitches become noticeable (unlikely for small 2D games, more relevant with large textures or audio).

### Refresh is manual - no file watching

**Current:** `ResourceManager::Load<T>(path, CacheMode::Refresh)` re-reads a resource only when a caller explicitly asks. Nothing notices that a file on disk changed.
**Concern:** Every iteration loop has to route through something that knows to pass `Refresh`. Editing an asset while the game runs does nothing until that code path happens to run again.
**Future:** Watch the asset directory (`ReadDirectoryChangesW` on Windows, or poll `last_write_time`) and refresh the affected resources automatically. The `Reload()` contract already preserves object identity, so nothing holding a resource pointer needs to know it happened.
**When:** When manually triggering a refresh becomes the annoying part of iterating on art or level data.

### Asset paths are relative to the executable

**Current:** `ResourceManager` resolves asset paths against an explicit asset root - the executable's directory, or the game's source directory in Debug. Assets are loose files copied next to the exe.
**Concern:** Loose files mean a shipped game's assets are readable and editable, and every load is a separate file open. There is also no way to load an asset from anywhere but the one root.
**Future:** A virtual file system: mount several roots (a pack file, a patch directory, the loose source tree) and resolve through them in order. `ResourceManager::SetAssetRoot` is the single seam this would replace.
**When:** When we ship a build to someone else, or need to override assets without replacing them.

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

### Documents can name each other, but the engine doesn't resolve it

**Current:** `SceneData` reads a flat list of entities whose properties are value types. `LevelSet::Levels` is a list of paths to other documents, but the engine sees plain strings - when the game instantiates the `LevelSet` it loads the current level's document itself, then carries on with the rest of the root document.
**Concern:** A document is not the whole truth about a scene. The three pointer properties across the games (`Grid*`, `Weapon*`, `const GridCoordinateSystem*`) have no text form either, so `Platformer::Initialize` still wires `SetWorld`, `SetFalcon` and `SetCursor` by hand.
**Future:** A property type the engine recognizes as a reference: load the named document, instantiate it, and hand back the entity. That needs a two-pass load (every entity has to exist before references resolve), a rule for load order, and cycle detection. It would also let Player and Falcon live in their own documents.
**When:** When hand-wiring in `Initialize` becomes the thing standing between a new entity type and a playable level.

### Draw order is instantiation order

**Current:** `Scene` renders entities in the order they were instantiated, so the order of a document's sections decides what draws on top. The Platformer's root document lists `LevelSet` first so that the level it pulls in lands behind Player, Falcon and Cursor.
**Concern:** Position in a file is an implicit way of saying "behind". Reordering sections for readability changes what covers what, and an entity spawned mid-game can only ever land on top.
**Future:** An explicit layer (or order) property on `Entity` that `Scene::Render` sorts by, so depth is stated in the document rather than implied by it.
**When:** When something spawned at runtime has to draw behind an existing entity, or a document's reading order and its draw order need to differ.

### No saving back to a scene document

**Current:** Documents are read-only. `PropertyParser<T>` turns text into a value; nothing turns a value back into text, and `TileLegend` only maps characters to tiles.
**Concern:** An in-game editor needs to write what it changed. Without saving, edits have to be typed into the file by hand.
**Future:** A matching `PropertyWriter<T>` (or a `ToString` beside each `Parse`), plus the reverse of `TileLegend`, and a document writer that keeps comments where it can. `FileSystem::WriteTextFile` is already there.
**When:** When we build an in-game editor, which is the only thing that needs it.

### Snake and Chess still hold their data in C++

**Current:** `CHESS_PIECES_DATA`, `TURN_LABEL_DATA` and `GAME_OVER_LABEL_DATA` are `EntityInfo` tables in `ChessConstants.h`; Snake does the same. Only the Platformer reads scene documents.
**Concern:** Two ways to describe the same thing. The Chess board's 32 pieces are exactly the repetitive data a document handles better than a C++ table.
**Future:** Move them to scene documents. Chess needs a `PropertyParser` for its `PieceColor` and `Type` enums; the rest is float, Vec2, Color and string, which are built in.
**When:** When either game is touched for another reason. Nothing is wrong with them today.
