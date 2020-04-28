#include "Shinobu/Renderer/RendererAPI.h"
#include "Shinobu/Core/Log.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace sh
{
    RendererAPI::API RendererAPI::m_api = RendererAPI::API::OpenGL;
    
    std::unique_ptr<RendererAPI> RendererAPI::Create()
    {
    
        switch (m_api)
        {
        case RendererAPI::API::None:
            SH_CORE_CRITICAL("RenderAPI None is not yet supported");
        default:
            return std::make_unique<OpenGLRendererAPI>();
            break;
        }
    }
}