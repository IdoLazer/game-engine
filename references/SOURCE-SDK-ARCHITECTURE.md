# Source SDK 2013 — Deep Dive Overview

## 0. What Is This? (SDK vs Engine, Entry Points, and the Full Pipeline)

### This Is NOT the Source Engine

The Source engine — `hl2.exe`, `engine.dll`, the renderer, the audio mixer, the networking core,
the filesystem — is **closed-source proprietary Valve code**. It is not in this repo. You cannot
read it, modify it, or build it.

What you have is the **Source SDK**: the source code for the **game logic DLLs** that plug into the
engine at runtime. You build exactly two things from this repo:

- **`client.dll`** — HUD, rendering hooks, prediction, interpolation, particle effects
- **`server.dll`** — entities, AI, physics interactions, game rules, damage

That's it. There is no `main()` in this codebase. The engine (`hl2.exe`) is the executable. It
loads your DLLs and calls into them through the interface/factory system.

### How the Pieces Connect at Runtime

```
┌──────────────────────────────────────────────────────────────┐
│  hl2.exe  (Valve's engine binary — provided via Steam)       │
│                                                              │
│  Loads closed-source engine DLLs:                            │
│  ├── engine.dll           (networking, BSP, frame loop)      │
│  ├── materialsystem.dll   (rendering, shader dispatch)       │
│  ├── vphysics.dll         (physics simulation)               │
│  ├── filesystem_stdio.dll (file I/O, VPK mounting)           │
│  ├── studiorender.dll     (model rendering)                  │
│  └── ... other engine DLLs                                   │
│                                                              │
│  Loads YOUR mod DLLs (built from this SDK):                  │
│  ├── client.dll  ◄── built from src/game/client/             │
│  └── server.dll  ◄── built from src/game/server/             │
│                                                              │
│  Communication happens through pure virtual interfaces:      │
│  Engine calls CreateInterface("ServerGameDLL009") on your    │
│  server.dll → gets IServerGameDLL* → calls DLLInit(),        │
│  GameFrame(), etc.  Your code calls the engine back through  │
│  IVEngineServer* obtained the same way.                      │
└──────────────────────────────────────────────────────────────┘
```

### Why Steam Is Required

The engine binary (`hl2.exe`) and all the closed-source engine DLLs are distributed through Steam
as **"Source SDK Base 2013 Singleplayer"** (AppID 243730) or **"Source SDK Base 2013 Multiplayer"**
(AppID 243750). You install these from Steam → Library → Tools.

Your mod's `gameinfo.txt` references this AppID:

```
SteamAppId  243730  // tells Steam which engine base to use
```

When you launch your mod, Steam ensures the engine files are present, sets up the app environment,
and runs `hl2.exe` with `-game "<your_mod_folder>"`. Steam is the delivery mechanism for the
engine you don't have source code to.

### The Entry Points (Where Your Code Gets Called)

Since there's no `main()`, the entry points are the interface implementations:

| Interface            | Your Implementation  | File                                  | What It Does                                                                        |
| -------------------- | -------------------- | ------------------------------------- | ----------------------------------------------------------------------------------- |
| `IServerGameDLL`     | `CServerGameDLL`     | `src/game/server/gameinterface.cpp`   | `DLLInit()`, `GameFrame()`, `LevelInit()`, `LevelShutdown()` — the server lifecycle |
| `IServerGameClients` | `CServerGameClients` | `src/game/server/gameinterface.cpp`   | `ClientConnect()`, `ClientDisconnect()`, `ClientCommand()` — per-player hooks       |
| Client entry         | `CHLClient`          | `src/game/client/cdll_client_int.cpp` | `Init()`, `HudUpdate()`, `FrameStageNotify()` — the client lifecycle                |

These are the **`main()` equivalents**. Start reading here to trace how the engine hands control
to your game code.

### The Complete Creation Pipeline

Here's the full workflow from source code to a running mod:

```
1. WRITE CODE (this SDK)
   └── Modify/add C++ in src/game/client/, server/, shared/

2. BUILD (Visual Studio)
   ├── Run src/creategameprojects.bat → generates games.sln
   ├── Open games.sln, build client_hl2 + server_hl2
   └── Output: game/mod_hl2/bin/client.dll + server.dll

3. CREATE CONTENT (Valve's tools — separate from this SDK)
   ├── Hammer Editor     → create maps (.vmf)
   ├── vbsp/vvis/vrad    → compile maps (.vmf → .bsp)
   ├── studiomdl         → compile models (.qc/.smd → .mdl)
   ├── vtex              → compile textures (.tga → .vtf)
   └── ... all output goes into game/mod_hl2/

4. CONFIGURE MOD
   └── game/mod_hl2/gameinfo.txt — mod name, Steam AppID, search paths

5. RUN
   ├── Install "Source SDK Base 2013 Singleplayer" in Steam
   ├── Launch: hl2.exe -game "path/to/game/mod_hl2"
   └── Engine loads your client.dll + server.dll, mounts your content
```

### Where Does Hammer Come In?

**Hammer** is Valve's level editor. It is NOT part of this SDK repo — it ships with the Source SDK
Base you install via Steam. It's a standalone GUI application where level designers build maps
using brushes (convex geometry), place entities (NPCs, triggers, lights, weapons), and wire up
entity I/O (Hammer's visual scripting: "when player touches this trigger, open that door").

Hammer outputs `.vmf` files (text-based map source). These then go through the compile pipeline:

```
Hammer (.vmf)
   → vbsp  (cuts geometry with BSP, builds spatial tree)
   → vvis  (calculates which areas can see which — PVS)
   → vrad  (bounces light, generates lightmaps)
   → .bsp  (final compiled map, placed in game/mod_hl2/maps/)
```

The source code for `vbsp`, `vvis`, and `vrad` IS in this SDK at `src/utils/`. The `fgdlib/`
folder contains the parser for `.fgd` files, which tell Hammer what entities your mod defines
(their properties, inputs, outputs). When you add a new entity class to your server code, you also
add it to a `.fgd` file so Hammer knows about it.

### So What Can You Actually Do With This?

- **Modify gameplay**: Change weapon damage, add new weapons, tweak AI behavior, add new entity types, change game rules
- **Add new NPCs**: Subclass `CAI_BaseNPC`, define schedules/tasks/conditions, add to `.fgd`
- **Change the HUD**: Modify or add `hud_*.cpp` files in the client
- **Write new shaders**: Add to `src/materialsystem/stdshaders/`
- **Study the architecture**: Learn interface patterns, networking, AI, entity systems from production code

What you **cannot** do: change the renderer core, modify the networking layer, alter the audio engine, or change how BSP works at the engine level. Those are in the closed-source engine DLLs.

---

## 1. Project Structure

The repo has two top-level directories:

| Directory | Purpose                                                                                                                                                                   |
| --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `game/`   | **Game content & mod templates** — config files, materials, scripts, scenes. Contains two mod templates: `mod_hl2` (singleplayer) and `mod_episodic` (Episode 1/2 based). |
| `src/`    | **All C++ source code** — the engine-game interface, game logic (client & server DLLs), libraries, shaders, and build tools.                                              |

Inside `src/`, the layout is:

```
src/
├── tier0/          # Platform abstraction (memory, threads, debug)
├── tier1/          # Core containers & utilities (UtlVector, KeyValues, CRC, interface system)
├── mathlib/        # Math library (vectors, matrices, SSE, quaternions)
├── vstdlib/        # Standard library extensions
├── public/         # ALL shared headers — the "contract" layer between engine and game DLLs
├── game/
│   ├── client/     # Client DLL (rendering, HUD, prediction, interpolation)
│   ├── server/     # Server DLL (entities, AI, physics interactions, game rules)
│   └── shared/     # Code compiled into BOTH client and server
├── materialsystem/ # Shader API (DX9) and standard shaders
├── vgui2/          # UI framework
├── raytrace/       # Ray-triangle intersection (used by lighting tools)
├── utils/          # Content pipeline tools (vbsp, vvis, vrad, etc.)
├── fgdlib/         # Hammer editor game definition parsing
└── thirdparty/     # External dependencies (protobuf, etc.)
```

**Build system**: Uses Valve's VPC (Valve Project Creator) to generate Visual Studio `.vcxproj` files. Run `src/creategameprojects.bat` to generate `src/games.sln`, which builds the client and server DLLs.

---

## 2. Main Components

### The Tier System (Layered Foundation)

Source uses a strict layering where each tier depends only on lower tiers:

- **Tier 0** — Platform primitives: memory allocation, threading, debug/assert, profiling, command line. Zero dependencies.
- **Tier 1** — Data structures and core utilities: `UtlVector`, `UtlMap`, `UtlBuffer`, `UtlRBTree`, `KeyValues`, `ConVar`, `BitBuf`, checksum functions. Most importantly: **the interface/factory system** (see below).
- **Tier 2** — Rendering helpers, file utilities, sound utilities.
- **Tier 3** — Model and choreography utilities.

### The Interface/Factory System (The Spine of the Engine)

This is **the single most important architectural pattern** in Source. All cross-DLL communication is mediated by:

1. **Pure virtual interfaces** with no data members (e.g., `IVEngineServer`, `IVEngineClient`, `IServerGameDLL`)
2. **Versioned string identifiers** (e.g., `"VEngineServer021"`, `"ServerGameDLL009"`)
3. A **`CreateInterface`** factory function exported by every DLL
4. At startup, each DLL receives factory function pointers to other DLLs and uses them to query for versioned interfaces

This makes the architecture **extremely modular** — the engine, game DLLs, physics, filesystem, and material system are all separate DLLs that only know each other through abstract interfaces.

### Client/Server DLL Split

The **server DLL** (`server.dll`) runs game simulation: entities, AI, physics, game rules, damage. The **client DLL** (`client.dll`) handles rendering, HUD, prediction, interpolation, and particle effects.

**Shared code** in `src/game/shared/` compiles into both DLLs using conditional compilation:

```cpp
#ifdef CLIENT_DLL
    #define CBaseEntity C_BaseEntity  // client prefix
#else
    // server uses CBaseEntity directly
#endif
```

### Networking (Send/Recv Tables)

Server entities declare `SendTable`s that describe which properties to replicate. Client entities declare matching `RecvTable`s. Proxy functions handle type conversion. The engine handles delta compression, prioritization, and bandwidth management automatically.

### Entity System

Deep single-inheritance hierarchy:

```
CBaseEntity → CBaseAnimating → CBaseFlex → CBaseCombatCharacter → CBasePlayer
                                          → CAI_BaseNPC
             → CBaseAnimating → CBaseCombatWeapon
```

Entities are registered with `LINK_ENTITY_TO_CLASS()`, serialized with `DECLARE_DATADESC()` / data maps, and networked with `DECLARE_SERVERCLASS()` / send tables.

### AI System (Server Only)

A rich **schedule/task/condition** framework in `src/game/server/ai_*.cpp`. NPCs select schedules based on conditions, execute tasks within schedules, and use components:

- **Navigator** — pathfinding
- **Senses** — perception (sight, sound, smell)
- **Motor** — movement execution
- **Memory** — enemy/knowledge tracking
- **Squad** — group coordination

### Physics

Wraps Havok-derived VPhysics through abstract interfaces (`IPhysics`, `IPhysicsEnvironment`, `IPhysicsObject`). Same factory pattern — the physics DLL is entirely separate from the game logic.

### Material/Shader System

`src/materialsystem/stdshaders/` contains 300+ files implementing Source's shaders across DX6–DX9. Key shader families:

- `LightmappedGeneric` — world brushes
- `VertexLitGeneric` — models
- `UnlitGeneric` — UI/sprites
- `Water`, `Eyes`, `Skin`, `Refract`, `Shadow`

Build scripts (`buildsdkshaders.bat`) compile `.fxc` HLSL into shader bytecode.

### Content Pipeline Tools

`src/utils/` contains the map compilation toolchain:

| Tool                 | Purpose                                                              |
| -------------------- | -------------------------------------------------------------------- |
| **vbsp**             | Compiles .VMF → .BSP (brush CSG, spatial partitioning)               |
| **vvis**             | Computes PVS (Potentially Visible Sets) — which leaves can see which |
| **vrad**             | Radiosity lighting — lightmaps, light bouncing                       |
| **captioncompiler**  | Closed caption compilation                                           |
| **phonemeextractor** | Lip-sync data from audio                                             |

---

## 3. Recommended Reading Order

### Phase 1: Understand the Foundations

1. **`src/public/tier1/interface.h`** — Read this first. The `CreateInterface` / `InterfaceReg` pattern is the key to everything. Every major system communicates through this.
2. **`src/public/tier0/`** and **`src/public/tier1/`** headers — Skim `dbg.h` (asserts/logging), `utlvector.h`, `utlmap.h`, `convar.h`. These are used everywhere.
3. **`src/public/eiface.h`** — The engine↔server contract. Read `IVEngineServer` and `IServerGameDLL` to understand how the engine talks to game code.
4. **`src/public/cdll_int.h`** — The engine↔client contract.

### Phase 2: Understand the Entity System

5. **`src/public/datamap.h`** — The reflection/serialization system. All entities use this for save/restore and Hammer I/O.
6. **`src/public/dt_send.h`** and **`src/public/dt_recv.h`** — Networking data tables.
7. **`src/game/server/baseentity.h`** — Root of all server entities.
8. **`src/game/shared/igamesystem.h`** — The self-registering singleton pattern for global systems.

### Phase 3: Trace a Concrete Example

9. **`src/game/server/hl2/weapon_crowbar.cpp`** (~200 lines) — A clean, complete weapon. Shows entity registration, activity tables, damage via ConVars, NPC AI integration (target leading), and animation events.
10. **`src/game/server/hl2/npc_zombie.cpp`** (~1000 lines) — Shows the AI schedule/task/condition system, sound envelopes, custom behaviors, and how NPCs extend the base framework.
11. **`src/game/shared/gamerules.h`** — How game modes are defined and how rules are networked to clients.

### Phase 4: Explore Systems of Interest

- **Rendering**: Start at `src/game/client/viewrender.cpp` → `src/materialsystem/stdshaders/`
- **Physics**: `src/public/vphysics_interface.h` → `src/game/server/physics.cpp`
- **AI in depth**: `src/game/server/ai_basenpc.h` → `ai_schedule.cpp` → `ai_navigator.cpp`
- **HUD**: `src/game/client/hud.cpp` → `hud_*.cpp` files in `src/game/client/hl2/`

---

## 4. Running & Experimenting

### Building

1. Run `src/creategameprojects.bat` to generate the VS solution
2. Open `src/games.sln` in Visual Studio (2013+ recommended; 2022 works with retargeting)
3. Build `client_hl2` and `server_hl2` (or the episodic variants)
4. Output DLLs go into `game/mod_hl2/bin/` (or `mod_episodic/bin/`)

### Running Your Mod

1. Install **Source SDK Base 2013 Singleplayer** (AppID 243730) via Steam → Tools
2. The `game/mod_hl2/gameinfo.txt` points to this base — it provides the engine binaries and HL2 content
3. Launch via Steam or directly with: `hl2.exe -game "<path_to_mod_hl2>"`

### Learning by Doing

- **`developer 1`** in console — see debug output
- **`ent_text`** — shows entity classnames, health, state in the 3D world
- **`ai_debug_*` cvars** — visualize NPC schedules, navigation, senses
- **`net_graph 3`** — visualize networking (see your send tables in action)
- **`mat_wireframe 1`** — see the rendering pipeline
- **Step through in a debugger** — attach VS to `hl2.exe`, set breakpoints in `CBaseEntity::Think()`, weapon fire functions, or AI schedule selection

---

## 5. Key Design Decisions & How They Differ From Other Engines

| Decision                | Source's Approach                                                                                                                      | How Others Differ                                                                                                                                       |
| ----------------------- | -------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **DLL Architecture**    | Game logic is in separate client/server DLLs, communicating with the engine through versioned abstract interfaces. Extreme modularity. | Unity/Unreal compile game code into the engine or use scripting layers. Godot uses modules compiled into a single binary.                               |
| **Client/Server Split** | Hard compile-time separation. Shared code uses `#ifdef CLIENT_DLL`. Server is authoritative.                                           | Unreal has a similar split but uses a unified `AActor` with replication markup. Unity has no built-in server authority model (until Netcode).           |
| **Entity System**       | Deep single-inheritance C++ class hierarchy. No component system.                                                                      | Unreal uses deep inheritance too (AActor → APawn → ACharacter). Unity and most modern engines use Entity-Component-System (ECS). Godot uses node trees. |
| **Networking**          | Declare-and-forget send/recv tables with proxy functions. Engine handles delta compression automatically.                              | Unreal's `UPROPERTY(Replicated)` is conceptually similar but more automated. ECS engines use snapshot-based networking.                                 |
| **Serialization**       | Custom `datamap_t` reflection system with macros — not a generic reflection system, purpose-built for save/restore and entity I/O.     | Unreal has `UPROPERTY()` with full reflection. Modern engines use code generation or runtime reflection.                                                |
| **AI**                  | Schedule/task/condition state machine with sensing, navigation, squad coordination — all in C++.                                       | Unreal uses behavior trees. Unity leaves AI to the developer. Most modern engines prefer behavior trees or GOAP over Source's schedule system.          |
| **Map Format**          | BSP (Binary Space Partitioning) with static lightmaps, PVS, and a 3-stage compile pipeline (vbsp → vvis → vrad).                       | Modern engines use dynamic GI, runtime occlusion culling, and scene graphs instead of BSP. BSP is very fast at runtime but limits dynamic geometry.     |
| **Scripting**           | None — all game logic is C++. Entity I/O (Hammer's input/output system) provides limited visual scripting for level designers.         | Unreal has Blueprints + C++. Unity has C#. Godot has GDScript/C#. Source's approach gives maximum performance but highest barrier to entry.             |
| **Shaders**             | Hand-written HLSL with DX version fallbacks (DX6 → DX9).                                                                               | Modern engines use node-based shader editors or unified shader languages.                                                                               |

---

## 6. Benefits & Disadvantages

### Benefits

- **Exceptional runtime performance** — BSP/PVS culling, static lightmaps, and lean C++ code make Source extremely efficient on limited hardware. This is why it ran well on 2004-era PCs and still runs on low-end machines today.
- **Rock-solid networking** — The send/recv table system with delta compression is battle-tested across CS:GO, TF2, L4D, and Dota 2. The client/server authority model prevents most cheats at the architecture level.
- **Superb AI framework** — The schedule/task/condition system is sophisticated and was ahead of its time. The NPC code in HL2 (combine soldiers, antlions, citizens) demonstrates professional-grade AI with squad tactics, speech, and dynamic behavior.
- **Modular DLL architecture** — The interface/factory pattern means you can swap out physics, rendering, or filesystem implementations without touching game code. This is a textbook example of dependency inversion at the systems level.
- **Content pipeline maturity** — The vbsp/vvis/vrad toolchain and Hammer editor are extremely well-integrated. Level designers get fast iteration with real lighting previews.
- **Real-world battle-tested codebase** — This powered Half-Life 2, Portal, L4D, CS:GO, and many more. You're reading production code, not academic examples.

### Disadvantages

- **No scripting language** — Every gameplay change requires C++ recompilation. This slows iteration dramatically compared to engines with scripting (Unreal Blueprints, Unity C#, Godot GDScript).
- **Deep inheritance hierarchy** — Adding new entity types means navigating 5-6 levels of base classes. Modern ECS approaches are more flexible for composition-based design.
- **BSP limitations** — Static world geometry, baked lighting, and the compile pipeline make large open worlds or fully dynamic environments impractical. Modern engines with dynamic GI and streaming have surpassed this.
- **Dated rendering pipeline** — DX9 era, no PBR by default, no compute shaders, no modern GI techniques (ray tracing, SSGI). The shader system requires manual DX version fallbacks.
- **Macro-heavy C++** — `DECLARE_CLASS`, `DECLARE_DATADESC`, `DECLARE_SERVERCLASS`, `LINK_ENTITY_TO_CLASS`, `DEFINE_INPUTFUNC`, etc. — powerful but opaque to newcomers. The codebase predates modern C++ features (no smart pointers, templates used sparingly, heavy use of raw pointers).
- **Documentation is sparse** — The code is the documentation. Header comments are sometimes excellent (like `interface.h`) and sometimes absent entirely.
- **Platform focus** — Primarily Windows/DirectX. Linux/Mac support exists but is secondary. No console code in this SDK.

---

## 7. What Makes This Codebase Especially Worth Studying

1. **The interface/factory pattern** is a masterclass in large-scale C++ architecture. Study how `CreateInterface`, versioned strings, and pure virtuals allow 10+ DLLs to communicate without compile-time coupling. This pattern is directly applicable to any plugin-based system you'll ever build.

2. **The networking model** (send tables, recv tables, prediction, interpolation) is one of the best implementations of client/server game networking ever shipped. The concepts here directly apply to any multiplayer game.

3. **The AI system** is a complete, professional implementation of schedule-based NPC behavior with sensing, pathfinding, squad tactics, and animation integration. Reading through a single NPC like `npc_combine_s.cpp` teaches more about game AI than most textbooks.

4. **The entity I/O system** (inputs, outputs, data descriptions) shows how to build a data-driven, designer-friendly entity system in C++ — connecting cause and effect without hardcoding.

5. **The content pipeline** (BSP compilation, PVS, radiosity) is a concrete implementation of classic real-time rendering algorithms. The source for `vbsp`, `vvis`, and `vrad` in `src/utils/` is where the theory from graphics textbooks meets practice.
