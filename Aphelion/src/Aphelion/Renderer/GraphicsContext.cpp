#include "Aphelion/Renderer/GraphicsContext.h"
#include "Aphelion/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace ap
{
    std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* windowHandle)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:
            //TODO: Implement null interface
            break;
        case RendererAPI::API::OpenGL:
            return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(windowHandle));
            break;
        }

        return std::unique_ptr<GraphicsContext>();
    }
}