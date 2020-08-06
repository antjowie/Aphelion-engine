#pragma once
#include "Aphelion/Core/Core.h"

namespace ap
{
    struct APHELION_API HostConfig
    {
        float rate = 1.f / 60.f;
    };

    struct APHELION_API HostStats
    {
        float rtt;
        float dropRate;
    };
}