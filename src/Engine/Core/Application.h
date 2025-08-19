#pragma once

#include "Rendering/Window.h"
#include "Rendering/Renderer2D.h"
#include "Input/Keyboard.h"

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
        virtual void Render() = 0;
        virtual void Shutdown() = 0;

        // Window configuration - override this in your game class
        virtual WindowConfig GetWindowConfig() const;

        void Close() { m_Running = false; }

    protected:
        Window *GetWindow() { return m_Window; }

    private:
        // Engine subsystem management
        bool InitializeSubsystems();
        void UpdateEngine();
        void RenderFrame();
        void ShutdownSubsystems();

        bool m_Running = true;
        bool m_Initialized = false;
        Window *m_Window = nullptr;

        // Delta time tracking
        double m_LastFrameTime = 0.0;
    };
}