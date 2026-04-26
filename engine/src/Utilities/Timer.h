#pragma once

#include <functional>

namespace Engine
{
    // Simple timer utility for game timing
    class Timer
    {
    public:
        using Callback = std::function<void()>;

        // --- Constructors & Destructors ---
        Timer();
        Timer(float interval, Callback callback, bool isLooping = true);

        // --- Lifecycle ---
        void Update(float deltaTime);

        // --- Control ---
        void Reset();
        void Stop();
        void Start();
        void SetInterval(float interval);
        void SetCallback(Callback callback);

        // --- Accessors ---
        bool IsActive() const;

    private:
        // --- Fields ---
        float m_interval;
        float m_accumulator;
        Callback m_callback;
        bool m_isLooping;
        bool m_isActive;
    };
}