#pragma once

#include <iostream>
#include <string>

namespace Engine
{
    class Application
    {
    public:
        Application() = default;
        virtual ~Application();
        virtual void Initialize() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;

        void Close() { m_Running = false; }
        void Run();

    private:
        bool m_Running = true;
    };
}