#pragma once
/** 
 * The Core header file contains macro definitions that are used through the whole code base.
 */

#include <memory>

#ifdef SH_DYNAMIC_LINK
    #ifdef SH_BUILD_DLL
        #define SHINOBU_API __declspec(dllexport)
    #else
        #define SHINOBU_API __declspec(dllimport)
    #endif
#else
    #define SHINOBU_API
#endif

constexpr inline SHINOBU_API unsigned BITSET(unsigned bit) { return 1 << bit; }