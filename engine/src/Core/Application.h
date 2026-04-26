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
    // --- Fields ---
    private:
        bool m_running = true;
        bool m_initialized = false;
        std::unique_ptr<Window> m_window;
        Scene m_scene;
        double m_lastFrameTime = 0.0;

    // --- Constructors & Destructors ---
    public:
        Application();
        virtual ~Application();

    // --- Engine Lifecycle ---
    public:
        bool InitializeEngine();
        void Run();
        void ShutdownEngine();

    // --- Game Interface ---
    public:
        virtual void Initialize() = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Shutdown() = 0;
        virtual WindowConfig GetWindowConfig() const;

    // --- Accessors ---
    protected:
        Window *GetWindow();
        Scene *GetScene();
        void Close();

    // --- Internal ---
    private:
        bool InitializeSubsystems();
        void UpdateEngine();
        void RenderFrame();
        void ShutdownSubsystems();
    };

    // Factory function — implemented by each game target
    Application *CreateApplication();
}