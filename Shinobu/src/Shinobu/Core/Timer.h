#pragma once

#include "Shinobu/Core/Core.h"

#include <chrono>

namespace sh
{
    class SHINOBU_API Timer
    {
    public:
        using Clock = std::chrono::high_resolution_clock;
    
        Timer()
            : m_begin(Clock::now())
        { }
    
        float Seconds()
        { 
            return std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - m_begin).count(); 
        }
    
    private:
        Clock::time_point m_begin;
    };
}