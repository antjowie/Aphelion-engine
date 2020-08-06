#include "Aphelion/Renderer/VertexArray.h"

#include "Aphelion/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace ap
{
    std::shared_ptr<VertexArray> VertexArray::Create()
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: SH_CORE_CRITICAL("VertexArray none not yet implemented"); 
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
        }

        return std::shared_ptr<VertexArray>();
    }
}