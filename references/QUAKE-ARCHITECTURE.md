# Quake Engine — Architecture Study Guide

## 1. Project Structure

The repo contains **three distinct codebases**:

| Directory   | What It Is                                                                                                                                                                              |
| ----------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `WinQuake/` | Original Quake engine — software renderer + GL renderer in one codebase. The monolithic single-player/multiplayer client-server. **Start here.**                                        |
| `QW/`       | **QuakeWorld** — a rewrite of the networking and client code for competitive online play. Split into `QW/client/` and `QW/server/`.                                                     |
| `qw-qc/`    | **QuakeC** game logic source — the scripting language that defines weapons, items, monsters, and game rules. Compiled to bytecode (`progs.dat`) and executed by a VM inside the engine. |

The entire engine is written in **C89 + x86 assembly**, built with Visual C++ 6.0 + MASM. No C++, no OOP, no dynamic memory allocation during gameplay.

---

## 2. Main Engine Components (WinQuake)

The engine breaks down into ~12 subsystems. Here's a map by file prefix:

| Prefix             | Subsystem                                                                | Key Files                                         |
| ------------------ | ------------------------------------------------------------------------ | ------------------------------------------------- |
| `sys_*`            | **Platform layer** — OS entry point, timers, file I/O                    | `sys_win.c` (Windows), `sys_linux.c`, `sys_dos.c` |
| `host*`            | **Frame coordinator** — the main loop that ticks every subsystem         | `host.c`, `host_cmd.c`                            |
| `cl_*`             | **Client** — input, server message parsing, entity interpolation         | `cl_main.c`, `cl_parse.c`, `cl_input.c`           |
| `sv_*`             | **Server** — game simulation, client management, physics                 | `sv_main.c`, `sv_phys.c`, `sv_user.c`             |
| `r_*`              | **Software renderer** — edge-based rasterizer                            | `r_main.c`, `r_edge.c`, `r_bsp.c`                 |
| `gl_*`             | **OpenGL renderer** — hardware-accelerated path                          | `gl_rmain.c`, `gl_rsurf.c`, `gl_mesh.c`           |
| `d_*`              | **Drawing primitives** — low-level pixel ops for software renderer       | `d_edge.c`, `d_scan.c`, `d_sprite.c`              |
| `net_*`            | **Networking** — multi-protocol abstraction (IPX, UDP, serial, loopback) | `net_main.c`, `net_dgrm.c`, `net_udp.c`           |
| `snd_*`            | **Sound** — DMA-based mixing, spatial audio                              | `snd_dma.c`, `snd_mix.c`, `snd_mem.c`             |
| `pr_*`             | **QuakeC VM** — bytecode interpreter for game logic                      | `pr_exec.c`, `pr_edict.c`, `pr_cmds.c`            |
| `cmd*`, `cvar*`    | **Console/config system** — command buffer, variables, key bindings      | `cmd.c`, `cvar.c`, `console.c`, `keys.c`          |
| `zone*`, `common*` | **Core utilities** — memory allocators, byte serialization, filesystem   | `zone.c`, `common.c`                              |
| `model*`           | **Asset loading** — BSP maps, alias models (.mdl), sprites               | `model.c`, `model.h`                              |
| `world*`           | **Collision** — BSP-based hull tracing                                   | `world.c`                                         |
| `vid_*`, `in_*`    | **Video/Input** — platform-specific display and input                    | `vid_win.c`, `in_win.c`                           |

### Layered Architecture Diagram

The engine is organized in implicit layers. Higher layers depend on lower ones; nothing reaches downward.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         GAME LOGIC (QuakeC VM)                         │
│  progs.dat bytecode ←→ pr_exec.c / pr_edict.c / pr_cmds.c             │
│  Weapons, items, monsters, triggers, game rules                        │
│  Calls engine builtins (spawn, traceline, precache, makevectors...)    │
└────────────────────────────┬────────────────────────────────────────────┘
                             │ calls builtins / reads & writes edict fields
┌────────────────────────────▼────────────────────────────────────────────┐
│                     SERVER SIMULATION (sv_*)                            │
│  sv_main.c ─── client management, precache tables, message routing     │
│  sv_phys.c ─── physics dispatch per movetype (TOSS, FLY, PUSH, STEP)  │
│  sv_user.c ─── process player input (usercmd_t)                        │
│  world.c ───── collision detection via BSP hull tracing                │
└───────┬──────────────────────────────┬──────────────────────────────────┘
        │ server messages (svc_*)      │ client messages (clc_*)
        │ entity state, stats, events  │ movement, commands
        ▼                              ▲
┌───────────────────────────────────────────────────────────────────────┐
│                     NETWORK PROTOCOL (net_*)                          │
│  protocol.h ── message types (svc_*, clc_*), delta flags (U_*)       │
│  common.c ──── MSG_Write*/Read* byte serialization                   │
│  net_main.c ── driver abstraction (loopback / UDP / IPX / serial)    │
│  net_dgrm.c ── datagram reliability layer (sequence, ack, retry)     │
│  QW: net_chan.c ── channel-based UDP with client-side prediction      │
└───────┬──────────────────────────────┬──────────────────────────────────┘
        │ entity snapshots + events    │ player input
        ▼                              │
┌───────────────────────────────────────────────────────────────────────┐
│                     CLIENT (cl_*)                                     │
│  cl_main.c ─── connection state machine, dynamic lights, lerp        │
│  cl_parse.c ── decode server messages → update client_state_t        │
│  cl_input.c ── sample keyboard/mouse → build usercmd_t               │
│  cl_tent.c ─── temporary entities (explosions, impacts, beams)       │
│  QW: cl_pred.c ── client-side movement prediction via pmove.c        │
└──────┬─────────────────┬──────────────────┬─────────────────────────┘
       │ entities +      │ listener pos +   │ view origin +
       │ view state      │ sound events     │ HUD stats
       ▼                 ▼                  ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────────────────────────┐
│  RENDERER    │  │    SOUND     │  │         UI / CONSOLE             │
│              │  │              │  │                                  │
│ Software:    │  │ snd_dma.c    │  │ console.c ── scrollback buffer   │
│  r_main.c   │  │ snd_mix.c    │  │ sbar.c ──── HUD (health, ammo)  │
│  r_edge.c   │  │ snd_mem.c    │  │ menu.c ──── main/options menus   │
│  r_bsp.c    │  │ cd_audio.c   │  │ keys.c ──── key binding dispatch │
│  d_*.c      │  │              │  │ screen.c ── view rect, fov       │
│              │  │ Spatial 3D   │  │                                  │
│ OpenGL:      │  │ mixing into  │  │ cmd.c ───── command buffer       │
│  gl_rmain.c │  │ DMA buffer   │  │ cvar.c ──── config variables     │
│  gl_rsurf.c │  │              │  │                                  │
│  gl_mesh.c  │  │              │  │                                  │
└──────┬───────┘  └──────┬───────┘  └──────────────┬───────────────────┘
       │                 │                          │
       ▼                 ▼                          ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    SHARED ENGINE CORE                                   │
│  host.c ────── frame coordinator (_Host_Frame ticks everything)        │
│  common.c ──── filesystem (PAK files), byte-order utils, parsing       │
│  zone.c ────── memory management (Zone / Hunk / Cache)                 │
│  model.c ───── asset loading (BSP, alias .mdl, sprites .spr)          │
│  mathlib.c ─── vec3 ops, AngleVectors, box-on-plane-side              │
│  crc.c ─────── checksums    wad.c ── texture WAD loading              │
│  chase.c ───── third-person camera                                     │
└──────────────────────────────┬──────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                 PLATFORM ABSTRACTION LAYER                              │
│                                                                         │
│  sys_win.c / sys_linux.c / sys_dos.c                                   │
│  ├── Entry point (WinMain / main)                                      │
│  ├── Sys_FloatTime() ── high-res timer                                 │
│  ├── Sys_FileOpen/Read/Write/Close ── file I/O                         │
│  ├── Sys_SendKeyEvents() ── pump OS message queue                      │
│  ├── Sys_Error() / Sys_Quit() ── fatal exit                           │
│  └── Sys_MakeCodeWriteable() ── self-modifying asm support             │
│                                                                         │
│  vid_win.c / vid_dos.c / gl_vidnt.c ── display init (DIB/DDraw/GL)    │
│  in_win.c / in_dos.c ── mouse, keyboard, joystick                     │
│  snd_win.c / snd_dos.c / snd_linux.c ── audio device                  │
│  cd_win.c / cd_linux.c ── CD audio playback                           │
│  net_wins.c / net_udp.c / net_ipx.c ── network sockets                │
└─────────────────────────────────────────────────────────────────────────┘
                               │
                               ▼
                    ┌─────────────────────┐
                    │    OS / Hardware     │
                    │  Win32, Linux, DOS   │
                    │  CPU, GPU, Sound HW  │
                    └─────────────────────┘
```

**How to read this diagram:**

- **Bottom-up**: The OS provides timers, files, windows, and sockets. The platform layer wraps these behind a uniform `Sys_*` / `vid_*` / `in_*` / `snd_*` API. The shared core provides memory, math, and asset loading. The client, server, renderer, sound, and UI sit on top of that. The network protocol connects client and server. QuakeC game logic sits at the very top, sandboxed in a VM.

- **The frame loop** (`host.c`) cuts vertically through all layers — it ticks server, client, renderer, and sound each frame. It's the glue that makes the layers into a running system.

- **Client and server are peers**, not parent-child. Even in single-player, they communicate through the same message protocol as if networked. The server simulates; the client presents. The network layer between them may be a loopback (same process) or real UDP.

- **Two renderers** (software and GL) are compile-time alternatives at the same layer. They consume the same `entity_t` / `refdef_t` data from the client and produce pixels through completely different paths.

---

## 3. Recommended Reading Order

### Phase 1: Understand the skeleton

1. **`quakedef.h`** — Master header. Every `#define` and engine limit lives here. Read all 336 lines; it's the Rosetta Stone.
2. **`sys_win.c`** → `WinMain()` — The actual entry point. Allocates one big memory block, calls `Host_Init()`, runs the main loop.
3. **`host.c`** → `_Host_Frame()` — **The heartbeat.** Every subsystem is ticked from here in a specific order. Understand this and you understand the whole engine.

### Phase 2: Understand the data flow

4. **`protocol.h`** — The network protocol defines (`svc_*`, `clc_*`, `U_*` delta flags). Only ~5 client→server message types vs. ~35 server→client. The client is intentionally dumb.
5. **`common.c`** — `MSG_Write*`/`MSG_Read*` byte serialization. Coordinates compress to 2 bytes (1/8 unit precision), angles to 1 byte (1.4° resolution).
6. **`server.h`** + **`client.h`** — The key data structures: `server_t`, `client_t`, `client_state_t`, `client_static_t`. The split between "persists across levels" vs. "wiped on level change" is important.

### Phase 3: Deep dives (pick based on interest)

7. **Memory**: `zone.c` — Three allocators (Zone, Hunk, Cache) carved from one `malloc`. A masterclass in embedded-style memory management.
8. **Rendering (software)**: `r_main.c` → `r_edge.c` — Edge-based rasterization with active edge tables. Historical and educational.
9. **Rendering (GL)**: `gl_rmain.c` → `gl_rsurf.c` — Immediate-mode OpenGL. Compare with the software path to see how the same geometry is rendered via two completely different approaches.
10. **Physics/Collision**: `sv_phys.c` + `world.c` — Movetype-based dispatch, BSP hull tracing. Bounding boxes are represented as mini BSP trees so collision code stays uniform.
11. **QuakeC VM**: `pr_exec.c` — A three-address-code bytecode interpreter in one big `switch`. 100K instruction runaway limit, 32-frame stack depth.
12. **Networking (QuakeWorld)**: `QW/client/cl_pred.c` — Client-side prediction, the key innovation that made online FPS playable.

### Phase 4: Game logic

13. **`qw-qc/defs.qc`** — All QuakeC global and entity field declarations.
14. **`qw-qc/weapons.qc`** — Hitscan via `traceline()`, projectiles via `spawn()` + velocity.
15. **`qw-qc/client.qc`** — Spawning, respawning, level transitions, intermission.

---

## 4. The Execution Flow

```
WinMain()                          ← sys_win.c: allocate memory, one big malloc
  └─ Host_Init()                   ← host.c: boot every subsystem in order
       └─ [Zone → Cmd → COM → Keys → Console → Progs → Model → Net → SV → Video → Renderer → Sound → CL]
  └─ Main Loop (forever):
       └─ Host_Frame(time)         ← host.c: THE frame function
            ├─ Sys_SendKeyEvents() ← pump OS message queue
            ├─ IN_Commands()       ← read mouse/joystick
            ├─ Cbuf_Execute()      ← process console command buffer
            ├─ Host_ServerFrame()  ← if server active:
            │    ├─ SV_RunClients()     ← read client inputs
            │    ├─ SV_Physics()        ← simulate world (movetype dispatch)
            │    └─ SV_SendClientMessages()
            ├─ CL_SendCmd()        ← send player input
            ├─ CL_ReadFromServer() ← parse server messages, update entities
            ├─ SCR_UpdateScreen()  ← RENDER (software or GL path)
            ├─ S_Update()          ← spatial audio mixing
            └─ CDAudio_Update()
```

Error recovery: any subsystem can call `Host_Error()` which does a `longjmp` back to the top of `Host_Frame()` — 1996's version of exception handling in C.

---

## 5. Key Subsystem Details

### 5.1 Memory Management (`zone.c`)

One `malloc` at startup, then three sub-allocators carved from that single block:

| Allocator | Purpose                                    | Strategy                                                                                                                     |
| --------- | ------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------- |
| **Zone**  | Small dynamic allocations (strings, cvars) | Rover allocator with free-block coalescing. Sentinel value `0x1d4a11` guards for corruption.                                 |
| **Hunk**  | Large level data (models, maps)            | Double-ended stack — low hunk grows up, high hunk grows down. Freed in bulk on level change via `Hunk_FreeToLowMark`.        |
| **Cache** | Evictable data (model meshes, sounds)      | LRU doubly-linked list. When hunk needs space, least-recently-used cache entries are freed and reloaded from disk on demand. |

Named allocations: every hunk block carries an 8-char name for debugging. `Hunk_Print` shows the full memory map. Sentinel value `HUNK_SENTINAL` is `0x1df001ed` — literally reads "1d fooled".

### 5.2 Networking

**Original Quake (`net_main.c`):**

- Driver abstraction via function pointer table (`net_drivers[]`). Supports loopback, serial, IPX, TCP/IP through one interface.
- Pre-allocated `qsocket_t` pool shuffled between `net_freeSockets` and `net_activeSockets` linked lists.
- Reliable + unreliable channels per socket with sequence tracking.
- Default port: 26000.

**QuakeWorld (`QW/client/net_chan.c`):**

- Complete rewrite. UDP-only, channel-based with sequence-numbered packets.
- `qport` field works around NAT routers remapping source ports mid-game.
- Reliable message retransmission built into the channel layer.
- **Client-side prediction** (`cl_pred.c`): replays unacknowledged user commands locally using shared `PlayerMove()` physics code (`pmove.c`). Eliminates the "laggy feel" of original Quake.
- **Delta-compressed entity updates** (`sv_ents.c`): PVS-filtered, only changed fields sent. Compact nail protocol for projectiles.

### 5.3 Software Renderer — Edge-Based Rasterization (`r_edge.c`)

Instead of per-pixel z-buffering, the software renderer uses an **active edge table (AET)** approach:

1. BSP tree generates polygon **edges** in back-to-front order.
2. For each scanline, a sorted list of active edges is maintained.
3. `R_LeadingEdge()` / `R_TrailingEdge()` detect when the frontmost surface changes.
4. Surface transitions emit **spans** — horizontal pixel runs belonging to one surface.
5. Spans are filled by `d_*.c` routines with texture-mapped pixels.

This is much more efficient than z-buffering on 1996 CPUs: it processes edges (not individual pixels), and BSP ordering eliminates most depth comparisons.

### 5.4 OpenGL Renderer (`gl_rmain.c`, `gl_rsurf.c`)

- Uses **OpenGL 1.x immediate mode** (`glBegin`/`glEnd`, `glVertex3fv`).
- `R_RotateForEntity` uses `glTranslatef`/`glRotatef` for entity transforms.
- Frustum culling via `R_CullBox()` — tests bounding boxes against 4 frustum planes.
- PVS-based visibility: `R_MarkLeaves` propagates visibility from the player's BSP leaf.
- Supports optional multitexturing, 8-bit 3DFX textures, translucent console.

### 5.5 Server Physics (`sv_phys.c`)

**Movement type dispatch** — each `MOVETYPE_*` has its own physics function:

| Movetype             | Behavior                         | Example             |
| -------------------- | -------------------------------- | ------------------- |
| `PUSH`               | No gravity, pushes other objects | Doors, platforms    |
| `TOSS`               | Gravity, stops on ground         | Gibs, dropped items |
| `STEP`               | Gravity with stair-stepping      | Monsters            |
| `FLY` / `FLYMISSILE` | No gravity, collision            | Rockets, scrags     |
| `BOUNCE`             | Gravity with bouncing            | Grenades            |
| `NOCLIP`             | No collision at all              | Debug / spectator   |

`SV_CheckVelocity` explicitly checks for NaN in velocity and origin — defensive programming against QuakeC bugs.

### 5.6 Collision Detection (`world.c`)

The most elegant trick: **bounding boxes are represented as 6-plane BSP hulls.** `SV_InitBoxHull` builds a BSP structure from an AABB so the same `SV_RecursiveHullCheck` code handles both world geometry and entity bounding boxes.

BSP maps compile **3 hull sizes**: hull 0 (point-sized), hull 1 (player-sized), hull 2 (large monsters). Entity size determines which hull to trace against.

### 5.7 QuakeC VM (`pr_exec.c`, `pr_edict.c`)

- **Three-address-code bytecode**: each instruction has operands `a`, `b`, `c`. Example: `OP_ADD_F: c->_float = a->_float + b->_float`.
- **Typed operations**: separate opcodes for float math, vector math, string ops, entity ops.
- **100K instruction runaway limit** per call prevents infinite loops.
- **32-frame call stack** with locals saved/restored on entry/exit.
- **Entity reuse delay**: `ED_Alloc` avoids reusing edicts freed within 0.5 seconds to prevent client-side interpolation artifacts.
- **Reflection**: the edict system can look up field definitions by name at runtime for save/load and entity spawning.

### 5.8 Console System (`cmd.c`, `cvar.c`, `console.c`)

- **Command buffer** (`Cbuf`): 8KB text buffer. `Cbuf_Execute` parses line-by-line, dispatches each as: command → cvar → forward to server.
- **Alias system**: named command macros (`cmdalias_t` linked list).
- **Cvar duality**: every cvar stores both `string` and `value` (float). Flags: `archive` (saved to config.cfg), `server` (broadcast to clients on change).
- **Console**: 16KB ring buffer with scrollback. `~` toggles; transparent overlay for notifications with 3-second timeout.

### 5.9 Asset Loading (`model.c`)

Three model types, discriminated by magic number at file start:

| Type   | Extension | Description                                          |
| ------ | --------- | ---------------------------------------------------- |
| Brush  | `.bsp`    | World geometry — BSP tree with PVS, lightmaps, hulls |
| Alias  | `.mdl`    | Character models — vertex-animated meshes            |
| Sprite | `.spr`    | Billboard sprites — flat images facing the camera    |

PVS (Potentially Visible Set) is run-length encoded and decompressed on demand. Model mesh data lives in the Cache allocator and can be evicted under memory pressure.

---

## 6. QuakeWorld vs. Original Quake

| Area            | Original Quake (WinQuake)                                    | QuakeWorld (QW/)                                                                              |
| --------------- | ------------------------------------------------------------ | --------------------------------------------------------------------------------------------- |
| **Networking**  | Multi-protocol (IPX, serial, UDP, loopback) via `net_main.c` | UDP-only channel-based (`net_chan.c`) with sequence numbers                                   |
| **Prediction**  | None — all movement waits for server round-trip              | Client-side prediction via shared `pmove.c` physics                                           |
| **Entity sync** | Full entity updates                                          | Delta-compressed + PVS-filtered (`sv_ents.c`)                                                 |
| **Client code** | `cl_main.c` (857 lines)                                      | `cl_main.c` (1536 lines) — adds server browsing, userinfo, skin downloading, spectator        |
| **Server code** | `sv_main.c` (318 lines)                                      | `sv_main.c` (1659 lines) — adds master servers, rcon, spectators, downloads, flood protection |
| **Game logic**  | QuakeC VM embedded in client-server binary                   | QuakeC VM in dedicated server only; client has no progs                                       |

---

## 7. QuakeC Language Overview

QuakeC is a simple C-like language compiled to bytecode. Key characteristics:

- **Types**: `float`, `vector` (3-component), `string`, `entity`, `void` — no integers, no structs, no arrays.
- **Entity fields** declared with `.` prefix: `.float health;`, `.vector velocity;`, `.void() think;`
- **Built-in functions**: `makevectors`, `traceline`, `precache_sound`, `spawn`, `setmodel`, `setorigin` bridge to the C engine.
- **Function pointers on entities** (`.think`, `.touch`, `.use`, `.blocked`) drive the event system.
- **Required entry points**: `StartFrame()`, `PlayerPreThink()`, `PlayerPostThink()`, `ClientConnect()`, `PutClientInServer()`, `ClientDisconnect()`.

**Compilation order** (`qw-qc/progs.src`):

```
defs.qc → subs.qc → combat.qc → items.qc → weapons.qc → world.qc →
client.qc → spectate.qc → player.qc → doors.qc → buttons.qc →
triggers.qc → plats.qc → misc.qc → server.qc
```

---

## 8. Running and Experimenting

**To build:** The project ships with `WinQuake.dsp` / `WinQuake.dsw` (VC6) and `qw.sln` solution files. For modern Visual Studio:

- Disable assembly files by setting `id386` to `0` in `quakedef.h` — you lose ~50% software renderer speed but the GL path is barely affected.
- Fix deprecated C library calls (`sprintf` → `sprintf_s`, etc.).
- Community ports like **QuakeSpasm** or **vkQuake** compile cleanly on modern toolchains and are better if your goal is to run and modify.

**To play/test:** You need original game data (`id1/pak0.pak` at minimum — shareware version works). Place the `id1/` folder next to the executable.

**Useful console variables for learning:**

| Command                                         | Effect                                                      |
| ----------------------------------------------- | ----------------------------------------------------------- |
| `host_speeds 1`                                 | Print time spent in server, client, and rendering per frame |
| `r_draworder 1`                                 | Software renderer draws back-to-front (visualize BSP order) |
| `r_drawflat 1`                                  | No textures, flat-shaded polygons (see the BSP structure)   |
| `r_speeds 1`                                    | Show polygon/surface counts                                 |
| `developer 1`                                   | Verbose debug output                                        |
| `sv_gravity` / `sv_friction` / `sv_maxvelocity` | Tweak physics in real-time                                  |

---

## 9. Key Architectural Decisions & Tradeoffs

### Single-binary client-server

Even single-player runs a local server. Client and server communicate through the same message protocol whether local or networked. Radical in 1996; became the standard for all FPS engines.

**Tradeoff:** Adds complexity to single-player but gives you multiplayer "for free" and makes the architecture testable.

### BSP as the universal spatial structure

The BSP tree is used for rendering (PVS), collision (hull tracing), sound occlusion, and entity spatial partitioning. Even AABBs are converted to BSP hulls for uniform collision code.

**Tradeoff:** Maps must be precompiled (slow iteration), geometry limited to convex brushes. But runtime performance is exceptional for the era.

### Three-tier memory (Zone / Hunk / Cache)

One `malloc` at startup, three sub-allocators. Zero fragmentation, zero allocation failure during gameplay.

**Tradeoff:** Requires careful discipline about allocation lifetimes.

### QuakeC virtual machine

Game logic in a sandboxed bytecode VM with runaway limits. Enables modding without recompiling.

**Tradeoff:** Much slower than native C, limited type system, but spawned Team Fortress, CTF, and the entire modding culture.

### Precache-everything model

All models and sounds declared at level start, assigned byte indices (max 256 each), fully loaded.

**Tradeoff:** No streaming, no hitches, predictable memory. But hard limits on content complexity.

### Edge-based software rasterization

Active edge table approach instead of z-buffering. Processes edges, not pixels.

**Tradeoff:** Far more efficient on 1996 CPUs, but doesn't generalize to arbitrary geometry.

---

## 10. Quake vs. Modern Engines

| Aspect          | Quake (1996)                                 | Modern Engines (Unreal, Unity)                             |
| --------------- | -------------------------------------------- | ---------------------------------------------------------- |
| **Language**    | C89 + x86 ASM                                | C++, C#, Rust                                              |
| **Memory**      | One malloc, three sub-allocators             | General-purpose allocators, GC                             |
| **Rendering**   | Software rasterizer OR fixed-function GL 1.x | Deferred rendering, PBR, compute shaders, Vulkan/DX12      |
| **Scene graph** | BSP tree (precompiled)                       | Octrees, BVH, dynamic spatial structures                   |
| **Physics**     | Custom movetype dispatch, BSP hull tracing   | PhysX, Havok, Bullet — full rigid body simulation          |
| **Scripting**   | QuakeC bytecode VM                           | C#, Lua, Blueprint, GDScript                               |
| **Networking**  | Entity delta compression, QW adds prediction | Prediction, rollback, interest management, ECS replication |
| **Audio**       | DMA mixing, basic spatial                    | FMOD, Wwise — HDR audio, convolution reverb                |
| **Threading**   | Entirely single-threaded                     | Job systems, render thread, async I/O                      |
| **Modularity**  | Subsystems via file naming convention        | Component architectures, ECS, plugin systems               |

### Why study Quake

- **Entire engine fits in your head.** ~80K lines of C. No abstractions hiding the metal.
- **Every system is visible.** No engine/editor split, no asset databases, no build systems. Read the C, understand the behavior.
- **Foundational patterns.** Client-server architecture, BSP rendering, delta compression, client-side prediction, console variable systems — all invented or popularized here — are still the basis of modern engines.
- **Constraints breed clarity.** 8 MB of RAM, 100 MHz CPU, no GPU. Every decision is deliberate and traceable.

### Limitations to be aware of

- **No threading.** Not representative of modern engine architecture.
- **Fixed-function rendering.** No shaders, no deferred rendering, no PBR.
- **Static worlds.** BSP maps precompiled. No destructible environments.
- **Primitive physics.** Simple movetype dispatch, not rigid body simulation.
- **No integrated editor.** Levels made in external tools (QuakeEd/Radiant).
