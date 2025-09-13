#pragma once

#include <functional>

// Simple timer utility for game timing
class Timer
{
public:
    using Callback = std::function<void()>;

private:
    float m_interval;
    float m_accumulator;
    Callback m_callback;
    bool m_isLooping;
    bool m_isActive;

public:
    Timer(float interval, Callback callback, bool isLooping = true)
        : m_interval(interval), m_accumulator(0.0f), m_callback(callback),
          m_isLooping(isLooping), m_isActive(true) {}

    void Update(float deltaTime);

    // Control methods
    void Reset();
    void Stop() { m_isActive = false; }
    void Start() { m_isActive = true; }
    void SetInterval(float interval) { m_interval = interval; }
    void SetCallback(Callback callback) { m_callback = callback; }
    bool IsActive() const { return m_isActive; }
};