#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Shinobu/Core/Log.h"

#include <glad/glad.h>

namespace sh
{
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         SH_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       SH_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          SH_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: SH_CORE_TRACE(message); return;
		}

		SH_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		//SH_PROFILE_FUNCTION();

#ifdef SH_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(unsigned x, unsigned y, unsigned width, unsigned height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}