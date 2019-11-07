#pragma once
/** 
 * The Core header file contains macro definitions that are used through the whole code base.
 */

#include <memory>

/////////////////////////////////////////////////
// Set up the DLL
//
// Not cross platform yet because I don't think __debugbreak is standard
#ifdef SH_DYNAMIC_LINK
    #ifdef SH_BUILD_DLL
        #define SHINOBU_API __declspec(dllexport)
    #else
        #define SHINOBU_API __declspec(dllimport)
    #endif
#else
    #define SHINOBU_API
#endif

/////////////////////////////////////////////////
// Set up the asserts
//
// It is not crossplatform yet because I don't think __debugbreak() is standard
#ifndef NDEBUG
    #define SH_ASSERT(x, msg) { if(!(x)) { SH_ERROR("Assertion Failed: {0}", msg); __debugbreak(); } }
    #define SH_CORE_ASSERT(x, msg) { if(!(x)) { SH_CORE_ERROR("Assertion Failed: {0}", msg); __debugbreak(); } }
#else
    #define SH_ASSERT(x, msg)
    #define SH_CORE_ASSERT(x, msg)
#endif

#define SH_VERIFY(x, msg) { x; SH_ASSERT(x,msg); }
#define SH_CORE_VERIFY(x, msg) { x; SH_CORE_ASSERT(x,msg); } 

/////////////////////////////////////////////////
// Helper functions
constexpr inline SHINOBU_API unsigned BIT(unsigned bit) { return 1 << bit; }