# Game Engine — Claude Instructions

## Project Overview

This is a **learning-focused C++ game engine** built as an educational journey into game engine architecture. The primary goal is understanding and learning, not commercial viability.

For the current architecture, project structure, entity system, type registry, and technology stack, see [README.md](README.md). The README is the single source of truth for what the engine looks like today. [FUTURE.md](FUTURE.md) tracks deferred architectural decisions and ideas worth revisiting.

---

## Build & Test Commands

```bash
# Configure (use --fresh if files were added/removed)
cmake --preset windows-x64-debug

# Build targets
cmake --build build/windows-x64-debug --target Snake
cmake --build build/windows-x64-debug --target Chess
cmake --build build/windows-x64-debug --target Tests

# Run
.\build\windows-x64-debug\bin\Snake\Snake.exe
.\build\windows-x64-debug\bin\Chess\Chess.exe

# Run tests
ctest --test-dir build/windows-x64-debug --output-on-failure
ctest --test-dir build/windows-x64-debug -R "Vec2"   # filter by name
```

---

## Developer Context

The developer is learning C++ through this project and has self-described "weak" C++ skills. Every interaction is a teaching opportunity. Mistakes and questions are expected and welcome.

---

## How to Collaborate

- **Be a learning partner, not just an implementer.** Challenge approaches constructively. If there's a better design, say so before writing code.
- **Explain the why.** Always walk through what you're doing and why — don't dump code without context.
- **Incremental changes only.** Never write large amounts of code in one go. Break work into small, testable steps and confirm each one before moving on.
- **Present options.** When multiple approaches exist, explain the trade-offs and ask for a preference rather than picking unilaterally.
- **Stop and ask** when you hit an ambiguity or design decision. Present options instead of making assumptions.
- **Reference learning materials** when using patterns or techniques — both the `references/` folder in this project (Hazel, Quake, Godot, Source SDK architectures) and external resources (C++ Core Guidelines, cppreference).
- **Don't optimize prematurely.** Clarity first, performance later.

### Implementation Workflow

1. Confirm you understand what the developer wants to achieve
2. Review how the change fits the current architecture (check FUTURE.md for related deferred items)
3. If there are better approaches, discuss them before implementing
4. Implement in small, explained increments
5. Ensure each increment compiles and works before continuing

---

## Code Style & Conventions

- **Clarity over cleverness** — readable code beats clever code
- **Small, focused classes** over monolithic ones
- **Engine namespace** — all engine code lives in `namespace Engine`
- **RAII** — proper resource management via constructors/destructors
- **Modern C++20** — use newer features (concepts, constexpr, ranges) when they make code clearer or safer; explain the benefit when you do
- **Test-driven** — new engine features should have tests in `tests/`. Write tests alongside or before implementation when practical
- **`tests/` covers engine modules only** — the `Tests` target links `Engine` and nothing else, and that stays true. Game code is not tested there, and the target never gains an include path or link dependency on a game to make a test compile. If something in a game looks worth testing, that's evidence it may belong in the engine — propose promoting it, don't reach into the game from `tests/`.

### Header vs Source

- **Trivial accessors** (one-liner get/set): inline in `.h`
- **Logic-bearing methods** (Initialize, Update, Render, anything non-trivial): declare in `.h`, implement in `.cpp`
- **Rule of Zero**: don't declare constructors, destructors, or copy/move operators unless the class manages a resource needing custom cleanup
- **Explicit `= default`**: only when the class also declares another special member function
- **Don't refactor for style alone** — only when already modifying a file for functional reasons

### Comments

- **Comments document the code as it stands**, not the process that produced it. They earn their place by carrying intent, a constraint, or a non-obvious *why* the code can't express itself.
- **Never comment the edit** — what changed, what moved, what a previous version did, or why a tweak was made while working. That context is invisible to anyone reading the file later, including a week from now, and it accumulates into noise. Commit messages carry the change; FUTURE.md carries the deferred decision.
- **Write every comment for a reader who has never seen the diff** — if it only makes sense to someone who watched the code being written, it doesn't belong in the file.
- **First ask whether it's needed at all.** Clear names are the documentation. A comment that restates the line under it — `// do this` above `DoThis()` — is noise, and it rots the moment the code changes. Before writing one, try making the code say it instead: rename, extract the condition, split the step out. Write the comment only when the code genuinely can't carry the meaning.
- **Keep them short.** One line by default, two or three only for a genuinely subtle constraint. A comment that needs a paragraph is a sign the code or the naming should change instead. Don't argue the design's merits and don't spell out the consequences of getting it wrong — that reasoning belongs in FUTURE.md or the commit message, not above every function.

---

## Documentation Discipline

- **README.md must stay current.** Any new module, architectural change, new technology, renamed target, or added/removed system requires a README update in the same commit. The Project Structure tree must reflect new files and folders.
- **FUTURE.md for deferred decisions.** Any time we say "this is fine for now, but later we should..." — add it to FUTURE.md immediately. Before planning new work, review FUTURE.md for related items.
