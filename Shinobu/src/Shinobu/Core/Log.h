#pragma once
/**
 * The logger is a central place to log messages
 */

// Ignore the warning about shared pointer needing a dll interface
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace sh 
{
	class SHINOBU_API Log
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
#define SH_CORE_TRACE(...)    ::sh::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SH_CORE_INFO(...)     ::sh::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SH_CORE_WARN(...)     ::sh::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SH_CORE_ERROR(...)    ::sh::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SH_CORE_CRITICAL(...) ::sh::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SH_TRACE(...)         ::sh::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SH_INFO(...)          ::sh::Log::GetClientLogger()->info(__VA_ARGS__)
#define SH_WARN(...)          ::sh::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SH_ERROR(...)         ::sh::Log::GetClientLogger()->error(__VA_ARGS__)
#define SH_CRITICAL(...)      ::sh::Log::GetClientLogger()->critical(__VA_ARGS__)

#ifdef _MSC_VER
    #pragma warning(pop)
#endif