#pragma once

#include "Aphelion/Core/Core.h"
#include "Aphelion/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace ap
{
    class APHELION_API OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

        virtual void Init();
        virtual void SwapBuffers();

    private:
        GLFWwindow* m_window;
    };

}