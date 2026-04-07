# Hazel Engine — Architecture Overview

## 1. Project Structure

The solution is organized into **4 projects** built with **Premake5** (Lua-based meta-build system that generates Visual Studio `.sln`/`.vcxproj` files):

| Project              | Type                    | Role                                                                                       |
| -------------------- | ----------------------- | ------------------------------------------------------------------------------------------ |
| **Hazel**            | Static Library (`.lib`) | The core engine — renderer, ECS, physics, scripting, events, platform abstraction          |
| **Hazelnut**         | Console App (`.exe`)    | The level editor — ImGui dockspace, scene hierarchy, viewport with gizmos, content browser |
| **Sandbox**          | Console App (`.exe`)    | Minimal standalone test app — hardcoded 2D rendering, no ECS or editor                     |
| **Hazel-ScriptCore** | .NET 4.7.2 DLL          | C# scripting API — loaded at runtime via embedded Mono                                     |

```
Hazel (engine .lib)
  ├── Hazelnut (editor .exe)  ──links──►  Hazel
  ├── Sandbox  (demo .exe)    ──links──►  Hazel
  └── Hazel-ScriptCore (.dll) ──loaded at runtime by Hazel's ScriptEngine
```

The `premake5.lua` at the root defines the workspace. `Dependencies.lua` centralizes all include paths, library paths, and library names (Vulkan SDK, Mono, SPIRV-Cross, etc.).

**Vendored dependencies** (all in `Hazel/vendor/`):
GLFW (windowing), Glad (OpenGL loader), ImGui + ImGuizmo (UI/gizmos), glm (math), entt (ECS), Box2D (2D physics), spdlog (logging), yaml-cpp (serialization), msdf-atlas-gen (text rendering), stb_image (image loading), Mono (C# runtime), filewatch (hot-reload), VulkanSDK/shaderc/SPIRV-Cross (shader compilation).

---

## 2. Main Components & What to Learn From Each

### A. Core Application Framework

**Files:** `Hazel/src/Hazel/Core/`

- **Application** — Singleton that owns the window, the layer stack, and the game loop. The loop: calculate delta time → run deferred main-thread tasks → update each layer → ImGui render pass → swap buffers.
- **Layer / LayerStack** — Ordered list of update-able, event-receiving objects. Regular layers are inserted at the front; overlays at the back. Events propagate **back-to-front** (overlays first), rendering is **front-to-back**.
- **EntryPoint** — `main()` is defined _inside the engine_. Client apps only implement a `CreateApplication()` factory function. This is a classic engine pattern — the engine controls the lifecycle.
- **Window** — Abstract interface with a `Create()` factory. Only `WindowsWindow` (GLFW-backed) exists.
- **Input** — Static class querying GLFW directly. No virtual dispatch.

**What to learn:** How to structure an engine so the _engine_ owns `main()` and the client just plugs in via a factory function and layers. How a layer stack provides a clean separation of concerns (game logic layer, debug overlay, ImGui layer).

### B. Event System

**Files:** `Hazel/src/Hazel/Events/`

A **synchronous, type-safe** event system:

- `Event` base class with an `EventType` enum and bitwise `EventCategory` flags.
- `EventDispatcher` — given an event, calls a typed callback only if the runtime type matches the template type.
- Events propagate through the layer stack; any layer can mark an event `Handled` to stop propagation.

**What to learn:** A simple, zero-allocation event system without requiring an event bus or message queue. The trade-off is that events must be handled immediately (no deferred processing). The code notes this as a known future improvement.

### C. Renderer

**Files:** `Hazel/src/Hazel/Renderer/`, `Hazel/src/Platform/OpenGL/`

**Three-layer abstraction:**

1. **RendererAPI** — pure virtual interface for draw calls, clears, viewport.
2. **RenderCommand** — static class delegating to a `RendererAPI*`. All engine code calls this.
3. **Renderer / Renderer2D** — high-level scene-oriented APIs.

Every GPU resource (`Buffer`, `VertexArray`, `Shader`, `Texture`, `Framebuffer`, `UniformBuffer`) follows the same pattern: **abstract base class + static `Create()` factory** that switches on the active API and returns the platform-specific subclass (e.g., `OpenGLVertexArray`).

**Renderer2D** is the workhorse — a fully static **batch renderer** supporting:

- **Quads** (textured/colored), **circles** (SDF in the fragment shader), **lines**, and **MSDF text**.
- Each primitive has its own vertex type, VBO, VAO, and shader. Quads share a pre-generated index buffer.
- Texture batching (up to 32 slots per batch; auto-flushes when full).
- Every vertex carries an `EntityID` (int) written to a `RED_INTEGER` framebuffer attachment for **editor mouse-picking**.

**Camera hierarchy:**

- `Camera` — minimal base (just a projection matrix).
- `OrthographicCamera` — standalone with position/rotation, used by sandbox.
- `EditorCamera` — 3D orbit camera (focal point, pitch/yaw, pan/zoom), used by the editor.

**What to learn:** How to build a renderer abstraction that can support multiple graphics APIs (even though only OpenGL is implemented). The factory pattern for GPU resources. How batch rendering works and why it's essential for 2D performance. How entity picking via framebuffer readback works.

### D. Scene / ECS

**Files:** `Hazel/src/Hazel/Scene/`

Built on **entt** (a header-only, high-performance ECS library). Hazel wraps it with:

- **Entity** — lightweight handle (entt ID + scene pointer) with template methods like `AddComponent<T>()`, `GetComponent<T>()`.
- **Scene** — owns the entt registry, a UUID→entity map, and the Box2D world pointer. Has three update modes:
  - **Edit** — rendering only (editor camera), no physics or scripts.
  - **Play** — copies the scene, starts Box2D + Mono scripts, uses in-scene camera.
  - **Simulate** — physics only (no scripts), editor camera.

**Components available:**
`IDComponent`, `TagComponent`, `TransformComponent`, `SpriteRendererComponent`, `CircleRendererComponent`, `CameraComponent`, `ScriptComponent` (C#), `NativeScriptComponent` (C++), `Rigidbody2DComponent`, `BoxCollider2DComponent`, `CircleCollider2DComponent`, `TextComponent`.

**Scene serialization** uses **YAML** (via yaml-cpp) — human-readable, diffable, with custom converters for glm types.

**What to learn:** How to wrap entt into a user-friendly API. How to structure a scene with edit/play/simulate modes (copying the scene before play so you can restore it on stop — a common editor pattern). How YAML serialization works for scene files.

### E. Physics

**Files:** `Hazel/src/Hazel/Physics/`

Thin integration with **Box2D**:

- The physics _world_ is created/stepped inside `Scene::OnRuntimeStart()` / `OnUpdateRuntime()`.
- Components (`Rigidbody2DComponent`, `BoxCollider2DComponent`, `CircleCollider2DComponent`) store physics material properties + a `void* RuntimeBody` holding the raw Box2D pointer.
- After each Box2D step, transforms are synced back from bodies to the ECS.

**What to learn:** How to integrate a third-party physics library in an ECS-driven engine. The pattern of storing runtime pointers in components and syncing state bidirectionally.

### F. C# Scripting (Mono)

**Files:** `Hazel/src/Hazel/Scripting/`, `Hazel-ScriptCore/`

- Embeds the **Mono JIT runtime** to host C# assemblies.
- **Two assemblies**: Hazel-ScriptCore.dll (engine API) + user game scripts DLL.
- **Hot-reload** via `filewatch` — monitors the DLL file, reloads on change.
- **ScriptGlue** registers ~20 internal calls (transforms, physics, input, text) using `mono_add_internal_call`.
- User scripts subclass `Entity`, override `OnCreate()` / `OnUpdate()`, and access components through C# wrappers.

**What to learn:** How to embed a managed runtime (Mono) in a C++ engine. How to expose engine functionality to scripts via internal calls. How field reflection works for editor-inspectable script properties.

### G. Editor (Hazelnut)

**Files:** `Hazelnut/src/`

A single `EditorLayer` (~776 lines) that provides:

- **ImGui dockspace** with menu bar.
- **Scene Hierarchy Panel** — entity tree + component property inspector.
- **Content Browser Panel** — file-system asset browser.
- **Viewport** — framebuffer-rendered scene with **ImGuizmo** translate/rotate/scale gizmos.
- **Toolbar** — Play / Simulate / Pause / Step with icon buttons.
- **Mouse picking** — reads entity ID from framebuffer at cursor position.
- **Keyboard shortcuts** — Ctrl+N/O/S/D, Q/W/E/R for gizmo modes, Delete for entity removal.
- **Debug overlays** — physics collider visualization, selection highlighting.

**What to learn:** How to build a full scene editor with ImGui. How docking, gizmos, and mouse-picking come together. How the play/stop workflow copies and restores scenes.

### H. Profiling

**File:** `Hazel/src/Hazel/Debug/Instrumentor.h`

An RAII-based profiler that outputs **Chrome Tracing JSON** format. Wrap scopes with `HZ_PROFILE_FUNCTION()`, open the output in `chrome://tracing`.

---

## 3. Recommended Reading Order

1. **Build system** — `premake5.lua`, `Dependencies.lua`, `Hazel/premake5.lua`. Understand what links to what.
2. **Entry point flow** — `Hazel/src/Hazel/Core/EntryPoint.h` → `Application.h` / `.cpp` → `Layer.h`. Trace how `main()` → `CreateApplication()` → `Run()` works.
3. **Events** — `Hazel/src/Hazel/Events/Event.h` → the concrete event files. Small and self-contained.
4. **Window + Input** — `Hazel/src/Hazel/Core/Window.h` → `Platform/Windows/WindowsWindow` → `Input.h`. See the platform abstraction pattern.
5. **Renderer abstraction** — `RendererAPI.h` → `RenderCommand.h` → `Renderer.h`. Then `Buffer.h`, `Shader.h`, `Texture.h` for the resource pattern.
6. **Renderer2D** — `Renderer2D.cpp`. The batch rendering implementation — the most instructive single file for understanding 2D rendering.
7. **ECS / Scene** — `Entity.h` → `Components.h` → `Scene.h` / `.cpp`. See how entt is wrapped and how the update loop dispatches to physics/scripts/rendering.
8. **Editor** — `EditorLayer.cpp`. See how everything comes together in a working application.
9. **Scripting** — `ScriptEngine.h` / `.cpp` and the `Hazel-ScriptCore/` C# sources. More advanced topic.

---

## 4. Running and Exploring the Code

1. **Setup:** Run `scripts/Setup.bat`. It downloads prerequisites (Premake, Vulkan SDK debug libs) and generates VS solution files.
2. **Build:** Open `Hazel.sln` in Visual Studio. The startup project is **Hazelnut** (the editor).
3. **Run Hazelnut:** Press F5. Open the sandbox project at `Hazelnut/SandboxProject/Sandbox.hproj`. You'll see the scene editor with a viewport, hierarchy, and content browser.
4. **Experiment in the editor:** Create entities, add components (sprites, cameras, physics bodies, scripts), hit Play to see physics and scripts run, hit Stop to restore.
5. **Run Sandbox:** Switch the startup project to Sandbox for a minimal rendering demo without any editor.
6. **Use the profiler:** Set `#define HZ_PROFILE 1` in the Instrumentor header, rebuild, run, and load the output JSON in `chrome://tracing`.
7. **Step through code:** Set breakpoints in `Application::Run()` to trace the game loop, in `Scene::OnUpdateRuntime()` to see physics/script/render dispatch, or in `Renderer2D::Flush()` to watch batch rendering.

---

## 5. Key Design Decisions vs. Other Engines

| Decision                 | Hazel's Approach                                               | Contrast                                                                                                                                                                 |
| ------------------------ | -------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Build system**         | Premake5 (Lua scripts → VS projects)                           | Unreal/Godot use CMake; Unity is proprietary. Premake is simpler to understand and customize.                                                                            |
| **Engine as static lib** | Client links to Hazel `.lib`; engine owns `main()`             | Godot compiles as a monolith; Unreal uses dynamic modules. Static linking is simpler for a learning project.                                                             |
| **ECS**                  | entt (external, header-only, archetypal)                       | Unity uses its own DOTS ECS; Unreal uses an actor/component model (not a pure ECS). entt is the gold standard for C++ ECS.                                               |
| **Renderer**             | Abstract API + factory pattern; only OpenGL implemented        | Commercial engines support multiple backends. Hazel's abstraction is _ready_ for Vulkan/DX but only OpenGL works — good for learning the pattern without the complexity. |
| **2D focus**             | Batch renderer for quads/circles/text; no 3D mesh pipeline yet | Most engines lead with 3D. Hazel's 2D-first approach keeps the renderer tractable for study.                                                                             |
| **Scripting**            | Embedded Mono for C#                                           | Unity also uses C# (via IL2CPP/Mono); Unreal uses C++/Blueprints; Godot uses GDScript/C#. Mono embedding is a great learning exercise.                                   |
| **Physics**              | Box2D integration via ECS components                           | Similar to what Unity does internally. Godot has built-in physics. The thin integration layer is easy to understand.                                                     |
| **Serialization**        | YAML via yaml-cpp                                              | Godot uses its own text format; Unreal uses binary `.uasset`. YAML is human-readable and version-control friendly.                                                       |
| **Editor**               | ImGui + ImGuizmo (immediate-mode GUI)                          | Unity/Unreal/Godot use retained-mode UIs. ImGui is fast to iterate on but harder to scale for complex UIs. Perfect for a learning project.                               |
| **Platform**             | Windows-only (GLFW + Win32)                                    | Commercial engines are cross-platform. The abstraction layers are in place for future platforms but only one implementation exists.                                      |

---

## 6. Benefits and Limitations

**Benefits for learning:**

- **Single-person scale.** The entire codebase is readable in a few sessions. No massive build times or sprawling module systems.
- **Clean abstractions.** The factory pattern for renderers, the layer stack, the event system — all textbook patterns implemented clearly.
- **Incremental complexity.** You can study each subsystem in isolation (events → window → rendering → ECS → physics → scripting → editor).
- **YouTube companion.** Every feature was built on camera with explanations — you can match code to video at [thecherno.com/engine](https://thecherno.com/engine).
- **Modern C++17.** Smart pointers, fold expressions, constexpr, structured bindings — good exposure to modern idioms.

**Limitations:**

- **Windows/OpenGL only.** The cross-platform and multi-API abstractions exist but aren't realized.
- **No 3D pipeline.** No mesh loading, materials, lighting, shadows. Purely 2D + debug.
- **No audio.** Listed as a goal but not implemented.
- **No asset pipeline.** No texture compression, no asset bundling, no resource management beyond raw file loading.
- **Single-threaded.** The game loop, rendering, and physics all run on the main thread. No job system.
- **ImGui editor scaling.** The entire editor is one ~776-line file. A production editor would need much more structure.
- **No networking, no AI, no animation system.**

**Bottom line:** Hazel is not a production engine — it's a **teaching engine**. Its value is in showing you _how the pieces of a game engine fit together_ at a scale you can fully comprehend. Once you understand how Hazel structures its renderer, ECS, physics, scripting, and editor, you'll be far better equipped to read and understand the (much larger) codebases of Godot, Unreal, or custom engines.
