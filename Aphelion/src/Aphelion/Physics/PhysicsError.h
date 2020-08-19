#pragma once
#include "Aphelion/Core/Core.h"

namespace ap
{
	/// Taken from PxErrorCode
    enum class PhysicsErrorCode 
    {
		NoError = 0,

		//! \brief An informational message.
		DebugInfo = 1,

		//! \brief a warning message for the user to help with debugging
		DebugWarning = 2,

		//! \brief method called with invalid parameter(s)
		InvalidParameter = 4,

		//! \brief method was called at a time when an operation is not possible
		InvalidOperation = 8,

		//! \brief method failed to allocate some memory
		OutOfMemory = 16,

		/** \brief The library failed for some reason.
		Possibly you have passed invalid values like NaNs, which are not checked for.
		*/
		InternalError = 32,

		//! \brief An unrecoverable error, execution should be halted and log output flushed
		Abort = 64,

		//! \brief The SDK has determined that an operation may result in poor performance.
		PerfWarning= 128,

		//! \brief A bit mask for including all errors
		MaskAll = -1
    };

    using PhysicsErrorLogCb = std::function<void(PhysicsErrorCode code, const char* message, const char* file, int line)>;
}