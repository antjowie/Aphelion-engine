#pragma once
#include "Shinobu/Core/Core.h"

namespace sh
{
    struct SHINOBU_API HostConfig
    {
        float rate = 1.f / 60.f;
    };

    struct SHINOBU_API HostStats
    {
        float rtt;
        float dropRate;
    };
}