#include "Aphelion/Renderer/Texture.h"

#include "Aphelion/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace ap
{
    TextureRef Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: SH_CORE_CRITICAL("Texture 2D not yet implemented");
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height);
        }

        //return std::shared_ptr<Texture2D>();
    }

    TextureRef Texture2D::Create(const char* path)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: SH_CORE_CRITICAL("Texture 2D not yet implemented");
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
        }

        //return std::shared_ptr<Texture2D>();
    }

    TextureRef ArrayTexture2D::Create(uint32_t x, uint32_t y, const char* path)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: SH_CORE_CRITICAL("Array Texture 2D None not yet implemented");
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLArrayTexture2D>(x,y,path);
        }
    }
}
