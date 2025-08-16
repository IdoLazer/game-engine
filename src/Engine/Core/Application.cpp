#include "Application.h"

namespace Engine
{
    void Application()
    {
        std::cout << "Application created." << std::endl;
    }

    Application::~Application()
    {
        std::cout << "Application destroyed." << std::endl;
    }

    void Application::Run()
    {
        Initialize();

        std::cout << "Starting application loop..." << std::endl;

        while (m_Running) // Replace with actual running condition
        {
            Update();
            Render();

            // For now, just run once to demonstrate
            break; // Remove this in a real application
        }

        Shutdown();
    }
}