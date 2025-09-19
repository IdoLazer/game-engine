#include "Timer.h"

namespace Engine
{
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

    void Timer::Reset()
    {
        m_accumulator = 0.0f;
        m_isActive = true;
    }
}