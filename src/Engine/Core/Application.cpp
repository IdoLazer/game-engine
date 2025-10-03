#include "Application.h"
#include "Rendering/Renderer2D.h"
#include <iostream>

namespace Engine
{
    Application::Application()
    {
        std::cout << "Application created." << std::endl;
    }

    Application::~Application()
    {
        if (m_Initialized)
        {
            ShutdownEngine();
        }
        std::cout << "Application destroyed." << std::endl;
    }

    bool Application::InitializeEngine()
    {
        if (m_Initialized)
        {
            std::cout << "Engine already initialized." << std::endl;
            return true;
        }

        std::cout << "Initializing engine..." << std::endl;

        // Initialize engine subsystems
        if (!InitializeSubsystems())
        {
            std::cerr << "Failed to initialize engine subsystems!" << std::endl;
            return false;
        }

        // Initialize game-specific code
        Initialize();

        m_Initialized = true;
        std::cout << "Engine initialization complete." << std::endl;
        return true;
    }

    void Application::Run()
    {
        if (!m_Initialized)
        {
            if (!InitializeEngine())
            {
                std::cerr << "Failed to initialize engine. Aborting." << std::endl;
                return;
            }
        }

        std::cout << "Starting main application loop..." << std::endl;

        // Initialize delta time tracking
        m_LastFrameTime = glfwGetTime();

        // Main application loop
        while (m_Running && !m_Window->ShouldClose())
        {
            // Calculate delta time
            double currentTime = glfwGetTime();
            float deltaTime = static_cast<float>(currentTime - m_LastFrameTime);
            m_LastFrameTime = currentTime;

            UpdateEngine();    // Update engine systems
            Update(deltaTime); // Update game logic with delta time
            RenderFrame();     // Render the frame
        }

        std::cout << "Application loop ended." << std::endl;
    }

    void Application::ShutdownEngine()
    {
        if (!m_Initialized)
            return;

        std::cout << "Shutting down engine..." << std::endl;

        // Shutdown game-specific code first
        Shutdown();

        // Shutdown engine subsystems
        ShutdownSubsystems();

        m_Initialized = false;
        std::cout << "Engine shutdown complete." << std::endl;
    }

    bool Application::InitializeSubsystems()
    {
        // Get window configuration from the game
        WindowConfig config = GetWindowConfig();

        // Create and initialize window with game-specific settings
        m_Window = new Window(config.title, config.width, config.height);
        if (!m_Window->Initialize())
        {
            std::cerr << "Failed to initialize window!" << std::endl;
            return false;
        }

        // Initialize 2D renderer
        Renderer2D::Initialize(m_Window->GetNativeWindow(), config.width, config.height);

        // Initialize input systems
        Keyboard::Initialize(m_Window->GetNativeWindow());
        Mouse::Initialize(m_Window->GetNativeWindow());

        std::cout << "Engine subsystems initialized successfully." << std::endl;
        std::cout << "Window: \"" << config.title << "\" (" << config.width << "x" << config.height << ")" << std::endl;
        return true;
    }

    WindowConfig Application::GetWindowConfig() const
    {
        // Default window configuration - games can override this
        WindowConfig config;

        config.title = "Default Game Engine Window";
        config.width = 800;
        config.height = 600;
        config.fullscreen = false;
        config.resizable = true;
        return config;
    }

    void Application::UpdateEngine()
    {
        // Update input systems
        Keyboard::Update();
        Mouse::Update();

        // Poll window events
        m_Window->PollEvents();
    }

    void Application::RenderFrame()
    {
        // Begin 2D rendering
        Renderer2D::BeginFrame();

        // Clear the screen with a dark background
        Renderer2D::Clear(Color(0.1f, 0.1f, 0.1f, 1.0f));

        // Render game content
        Render();

        // End 2D rendering
        Renderer2D::EndFrame();

        // Present the frame
        m_Window->SwapBuffers();
    }

    void Application::ShutdownSubsystems()
    {
        // Shutdown renderer
        Renderer2D::Shutdown();

        // Clean up window (this also cleans up GLFW)
        if (m_Window)
        {
            delete m_Window;
            m_Window = nullptr;
        }

        std::cout << "Engine subsystems shut down." << std::endl;
    }
}