#pragma once

// Main Engine Header
// This is the only header a game needs to include to use the engine

// Core Engine Systems
#include "Core/Application.h"
#include "Core/Scene.h"

// Rendering System
#include "Rendering/Window.h"
#include "Rendering/Renderer2D.h"
#include "Rendering/Camera2D.h"

// Input System
#include "Input/KeyCodes.h"
#include "Input/MouseCodes.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

// Math
#include "Math/Vec2.h"

// Patterns
#include "Patterns/Command/Command.h"
#include "Patterns/Command/CommandQueue.h"

// Graphics
#include "Graphics/Color.h"

// Entity System
#include "Entity/Entity.h"
#include "Entity/GridEntity.h"
#include "Entity/GridTile.h"

// World Systems
#include "World/GridCoordinateSystem.h"
#include "World/Grid.h"
#include "World/TileMap.h"

// Utilities
#include "Utilities/Timer.h"

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
