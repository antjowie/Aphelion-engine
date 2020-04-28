#pragma once
/** 
 * The Core header file contains macro definitions that are used through the whole code base.
 */

#include <memory>

/**
 * Platform detection
 */
#ifdef _WIN32
	/* Windows x64/x86 */
    #ifdef _WIN64
    	/* Windows x64  */
        #define SH_PLATFORM_WINDOWS
    #else
    	/* Windows x86 */
        #error "x86 Builds are not supported!"
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #error "Others platforms are not yet supported"
 //#include <TargetConditionals.h>
///* TARGET_OS_MAC exists on all the platforms
// * so we must check all of them (in this order)
// * to ensure that we're running on MAC
// * and not some other Apple platform */
//#if TARGET_IPHONE_SIMULATOR == 1
//#error "IOS simulator is not supported!"
//#elif TARGET_OS_IPHONE == 1
//#define HZ_PLATFORM_IOS
//#error "IOS is not supported!"
//#elif TARGET_OS_MAC == 1
//#define HZ_PLATFORM_MACOS
//#error "MacOS is not supported!"
//#else
//#error "Unknown Apple platform!"
//#endif
// /* We also have to check __ANDROID__ before __linux__
//  * since android is based on the linux kernel
//  * it has __linux__ defined */
//#elif defined(__ANDROID__)
//#define HZ_PLATFORM_ANDROID
//#error "Android is not supported!"
//#elif defined(__linux__)
//#define HZ_PLATFORM_LINUX
//#error "Linux is not supported!"
//#else
//	/* Unknown compiler/platform */
//#error "Unknown platform!"
#endif // End of platform detection

/**
 * Set up the DLL
 */
#ifdef SH_DYNAMIC_LINK
    #ifdef SH_BUILD_DLL
        #define SHINOBU_API __declspec(dllexport)
    #else
        #define SHINOBU_API __declspec(dllimport)
    #endif
#else
    #define SHINOBU_API
#endif

//#define KEEP_ASSERT

/**
 * Set up the asserts
 *
 * It is not crossplatform yet because I don't think __debugbreak() is standard
 */
#if defined(SH_DEBUG) || defined(KEEP_ASSERT)
    #define SH_ASSERT(x, msg) { if(!(x)) { SH_ERROR("Assertion Failed: {0}", msg); __debugbreak(); } }
    #define SH_CORE_ASSERT(x, msg) { if(!(x)) { SH_CORE_ERROR("Assertion Failed: {0}", msg); __debugbreak(); } }
#else
    #define SH_ASSERT(x, msg)
    #define SH_CORE_ASSERT(x, msg)
#endif

/**
 * Verify is used if you still want to execute the arguments in release but not 
 * stop execution of the project if failed
 */
#define SH_VERIFY(x, msg) { x; SH_ASSERT(x,msg); }
#define SH_CORE_VERIFY(x, msg) { x; SH_CORE_ASSERT(x,msg); } 

constexpr inline SHINOBU_API unsigned BIT(unsigned bit) { return 1 << bit; }

#define SH_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)