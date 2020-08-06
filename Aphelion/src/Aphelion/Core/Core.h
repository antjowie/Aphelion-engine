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
        #define AP_PLATFORM_WINDOWS
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
#ifdef AP_DYNAMIC_LINK
    #ifdef AP_BUILD_DLL
        #define APHELION_API __declspec(dllexport)
    #else
        #define APHELION_API __declspec(dllimport)
    #endif
#else
    #define APHELION_API
#endif

//#define KEEP_ASSERT

/**
 * Set up the asserts
 *
 * It is not crossplatform yet because I don't think __debugbreak() is standard
 
 * Verify is used if you still want to execute the arguments in release but not 
 * stop execution of the project if failed
 */
#if defined(AP_DEBUG) || defined(KEEP_ASSERT)
    #define AP_ASSERT(x, msg) { if(!(x)) { AP_ERROR("Assertion Failed: {0}", msg); __debugbreak(); } }
    #define AP_CORE_ASSERT(x, msg) { if(!(x)) { AP_CORE_ERROR("Assertion Failed: {0}", msg); __debugbreak(); } }
    #define AP_VERIFY(x, msg) { AP_ASSERT(x,msg); }
    #define AP_CORE_VERIFY(x, msg) { AP_CORE_ASSERT(x,msg); } 
#else
    #define AP_ASSERT(x, msg)
    #define AP_CORE_ASSERT(x, msg)
    #define AP_VERIFY(x, msg) { x; }
    #define AP_CORE_VERIFY(x, msg) { x; } 
#endif

#define AP_BIND_FN(fn) std::bind(&fn, this, std::placeholders::_1)

/**
 * Utilities
 */
namespace ap
{
    constexpr inline APHELION_API unsigned Bit(unsigned bit) { return 1 << bit; }

    class APHELION_API NonCopyable
    {
    public:
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
    };

    template <typename T> 
    class Reverse
    {
    public:
        explicit Reverse(T& container) : m_container{ container } {}
        auto begin() const { return std::rbegin(m_container); }
        auto end() const { return std::rend(m_container); }
    private:
        T& m_container;
    };
}