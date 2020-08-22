#pragma once

#include "Aphelion/Core/Core.h"

#include <chrono>

namespace ap
{
    class APHELION_API Timestep
    {
    public:
        Timestep(float seconds) : m_seconds(seconds) {}

        operator float() const { return m_seconds; }
        float Seconds() const { return m_seconds; }
        float MilliSeconds() const { return m_seconds * 1000.f; }

    private:
        float m_seconds;
    };

    /**
     * Global accessible time value
     */
    struct APHELION_API Time
    {
        static Timestep dt;
        static unsigned frameCount;
    };

    class APHELION_API Timer
    {
    public:
        using Clock = std::chrono::high_resolution_clock;
    
        Timer()
            : m_begin(Clock::now())
            , m_last(m_begin)
        { }
    
        Timestep Total() const
        {
            return Timestep((Clock::now() - m_begin).count() * 1e-9f);
        }

        Timestep Elapsed() const
        {
            return Timestep((Clock::now() - m_last).count() * 1e-9f); 
        }

        Timestep Reset()
        {
            auto ts = Elapsed();
            m_last = Clock::now();
            return ts;
        }
    
    private:
        Clock::time_point m_begin;
        Clock::time_point m_last;
    };
}