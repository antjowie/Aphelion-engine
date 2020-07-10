#pragma once

#include "Shinobu/Core/Core.h"

#include <chrono>

namespace sh
{
    class SHINOBU_API Timestep
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
    struct SHINOBU_API Time
    {
        static Timestep dt;
    };

    class SHINOBU_API Timer
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

        Timer& Reset()
        {
            m_last = Clock::now();
            return *this;
        }
    
    private:
        Clock::time_point m_begin;
        Clock::time_point m_last;
    };
}