#include "Shinobu/Renderer/Texture.h"

#include "Shinobu/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace sh
{
    std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: SH_CORE_CRITICAL("Texture 2D not yet implemented");
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height);
        }

        //return std::shared_ptr<Texture2D>();
    }

    std::shared_ptr<Texture2D> Texture2D::Create(const char* path)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: SH_CORE_CRITICAL("Texture 2D not yet implemented");
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
        }

        //return std::shared_ptr<Texture2D>();
    }
}