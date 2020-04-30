#pragma once
#include "Shinobu/Core/Core.h"

namespace sh
{
    class SHINOBU_API Timestep
    {
    public:
        Timestep(float seconds) : m_seconds(seconds) {}

        operator float() const { return m_seconds; }
        float Seconds() const { return m_seconds; }
        float MiliSecond() const { return m_seconds * 1000.f; }

    private:
        float m_seconds;
    };
}