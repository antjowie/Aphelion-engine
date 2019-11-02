#pragma once

#ifdef SH_DYNAMIC_LINK
    #ifdef SH_BUILD_DLL
        #define SHINOBU_API __declspec(dllexport)
    #else
        #define SHINOBU_API __declspec(dllimport)
    #endif
#else
    #define SHINOBU_API
#endif