#pragma once

// Main Engine Header
// This is the only header a game needs to include to use the engine

// Core Engine Systems
#include "Core/Application.h"
#include "Core/EntryPoint.h" // This brings in main()

// Rendering System
#include "Rendering/Window.h"
#include "Rendering/Renderer2D.h"
#include "Rendering/Camera2D.h"

// Input System
#include "Input/Keyboard.h"

// This header provides everything a game needs:
// 1. Application base class to inherit from
// 2. All engine subsystems (Renderer2D, Keyboard, etc.)
// 3. The main() entry point via EntryPoint.h
//
// Game usage:
// #include "Engine/Engine.h"
//
// class MyGame : public Engine::Application { ... };
//
// Engine::Application* Engine::CreateApplication() { return new MyGame(); }
