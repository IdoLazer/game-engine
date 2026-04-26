#include "Timer.h"

namespace Engine
{
    // --- Constructors & Destructors ---

    Timer::Timer()
        : m_interval(0.0f), m_accumulator(0.0f), m_isLooping(true), m_isActive(false) {}

    Timer::Timer(float interval, Callback callback, bool isLooping)
        : m_interval(interval), m_accumulator(0.0f), m_callback(callback),
          m_isLooping(isLooping), m_isActive(true) {}

    // --- Lifecycle ---

    void Timer::Update(float deltaTime)
    {
        if (!m_isActive)
            return;

        m_accumulator += deltaTime;
        if (m_accumulator >= m_interval)
        {
            if (m_callback)
            {
                m_callback();
            }

            if (m_isLooping)
            {
                m_accumulator = 0.0f;
            }
            else
            {
                m_isActive = false; // One-shot timer stops after first execution
            }
        }
    }

    // --- Control ---

    void Timer::Reset()
    {
        m_accumulator = 0.0f;
        m_isActive = true;
    }

    void Timer::Stop() { m_isActive = false; }
    void Timer::Start() { m_isActive = true; }
    void Timer::SetInterval(float interval) { m_interval = interval; }
    void Timer::SetCallback(Callback callback) { m_callback = callback; }

    // --- Accessors ---

    bool Timer::IsActive() const { return m_isActive; }
}