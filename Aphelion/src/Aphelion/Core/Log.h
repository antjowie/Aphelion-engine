#pragma once
/**
 * The logger is a central place to log messages
 */

#include "Aphelion/Core/Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

// Ignore the warning about shared pointer needing a dll interface
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif

namespace ap 
{
	class APHELION_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_ClientLogger;
	};
}

// Core log macros
#define AP_CORE_TRACE(...)    ::ap::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define AP_CORE_INFO(...)     ::ap::Log::GetCoreLogger()->info(__VA_ARGS__)
#define AP_CORE_WARN(...)     ::ap::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define AP_CORE_ERROR(...)    ::ap::Log::GetCoreLogger()->error(__VA_ARGS__)
#define AP_CORE_CRITICAL(...) ::ap::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define AP_TRACE(...)         ::ap::Log::GetClientLogger()->trace(__VA_ARGS__)
#define AP_INFO(...)          ::ap::Log::GetClientLogger()->info(__VA_ARGS__)
#define AP_WARN(...)          ::ap::Log::GetClientLogger()->warn(__VA_ARGS__)
#define AP_ERROR(...)         ::ap::Log::GetClientLogger()->error(__VA_ARGS__)
#define AP_CRITICAL(...)      ::ap::Log::GetClientLogger()->critical(__VA_ARGS__)

#ifdef _MSC_VER
    #pragma warning(pop)
#endif