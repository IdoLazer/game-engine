#pragma once

#include <memory>
#include "Scene.h"
#include "Rendering/Window.h"
#include "Rendering/Renderer2D.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

namespace Engine
{
    struct WindowConfig
    {
        std::string title = "Game Engine";
        int width = 800;
        int height = 600;
        bool fullscreen = false;
        bool resizable = true;
    };

    class Application
    {
    public:
        Application();
        virtual ~Application();

        // Engine lifecycle methods
        bool InitializeEngine();
        void Run();
        void ShutdownEngine();

        // Game-specific methods for derived classes to implement
        virtual void Initialize() = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Render() const = 0;
        virtual void Shutdown() = 0;

        // Window configuration - override this in your game class
        virtual WindowConfig GetWindowConfig() const;

        void Close() { m_running = false; }

    protected:
        Window *GetWindow() { return m_window.get(); }
        Scene *GetScene() { return &m_scene; }

    private:
        // Engine subsystem management
        bool InitializeSubsystems();
        void UpdateEngine();
        void RenderFrame();
        void ShutdownSubsystems();

        bool m_running = true;
        bool m_initialized = false;
        std::unique_ptr<Window> m_window;
        Scene m_scene;

        // Delta time tracking
        double m_lastFrameTime = 0.0;
    };

    // Factory function that must be implemented by the game
    // The game will return an instance of their Application-derived class
    Application *CreateApplication();
}