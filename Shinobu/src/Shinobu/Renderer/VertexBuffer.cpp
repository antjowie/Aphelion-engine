#include "Shinobu/Renderer/VertexBuffer.h"

#include "Shinobu/Renderer/RendererAPI.h"

#include "Platform/OpenGL/OpenGLVertexBuffer.h"

namespace sh
{
    void VertexBuffer::AddElement(const Element& elem)
    {
        m_elements.push_back(elem);
    }

    const std::vector<VertexBuffer::Element>& VertexBuffer::GetElements() const
    {
        return m_elements;
    }
    
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: SH_CORE_CRITICAL("Buffer NONE is not yet implemented");
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(size);
        }
    }

    std::shared_ptr<VertexBuffer> VertexBuffer::Create(const float* vertices, uint32_t size)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: SH_CORE_CRITICAL("Buffer NONE is not yet implemented");
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
        }
    }

    std::shared_ptr<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: SH_CORE_CRITICAL("Index Buffer NONE is not yet implemented");
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLIndexBuffer>(indices, count);
        }
    }
}