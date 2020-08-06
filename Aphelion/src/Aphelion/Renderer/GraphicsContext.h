#pragma once
#include "Aphelion/Core/Core.h"

namespace ap
{
    /**
     * A context is what a render api uses to communicate with the os
     * The reason why we abstract this is because different graphics apis
     * have different contexts. 
     * 
     * At first this was done in the window, but that is not feasible. 
     * I at the time of writing I use GLFW which supports OpenGL and Vulkan.
     * Since the context differs for each render api, we have to abstract that.
     */
    class APHELION_API GraphicsContext
    {
    public:
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        static std::unique_ptr<GraphicsContext> Create(void* windowHandle);
    };
}