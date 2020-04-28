#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace sh
{
    class SHINOBU_API OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

        virtual void Init();
        virtual void SwapBuffers();

    private:
        GLFWwindow* m_window;
    };

}