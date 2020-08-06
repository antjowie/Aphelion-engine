#include "Aphelion/Renderer/Shader.h"
#include "Aphelion/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace ap
{
    std::shared_ptr<Shader> Shader::Create(const std::string& filepath)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:
            //SH_CORE_CRITICAL()
            break;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(filepath.c_str());
            break;
        }
    }

    std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:
            //SH_CORE_CRITICAL()
            break;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(name.c_str(), vertexSrc.c_str(), fragmentSrc.c_str());
            break;
        }
    }
}