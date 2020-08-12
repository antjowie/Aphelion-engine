#pragma once
#include "Aphelion/Core/Core.h"

#include <random>

namespace ap
{
    class APHELION_API PRNG
    {
    public:
        PRNG(unsigned seed) : m_engine(seed) {}
        unsigned Get() { return m_engine(); }

    private:
        std::mt19937_64 m_engine;
    };
}