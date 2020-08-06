#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ap
{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_window(windowHandle)
    {
        AP_CORE_ASSERT(m_window, "Window handle can't be null");
    }

    void OpenGLContext::Init()
    {
        // Init GLFW
        glfwMakeContextCurrent(m_window);

        // Init Glad
        gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));
        glfwMakeContextCurrent(m_window);

        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        AP_CORE_ASSERT(status, "Failed to initialize Glad!");

        AP_CORE_INFO("OpenGL Info:");
        AP_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
        AP_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
        AP_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_window);
    }
}