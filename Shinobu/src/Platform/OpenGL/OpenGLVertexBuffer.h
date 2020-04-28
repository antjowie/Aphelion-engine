#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/VertexBuffer.h"

namespace sh
{
    class SHINOBU_API OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        struct Element
        {
            unsigned index;
            unsigned count;
            unsigned dataType;
            unsigned normalized;
            unsigned stride;
            unsigned offset;
        };

    public:
        OpenGLVertexBuffer(uint32_t size);
        OpenGLVertexBuffer(const float* vertices, uint32_t size);
        virtual ~OpenGLVertexBuffer() override final;

        virtual void Bind() const override final;
        virtual void Unbind() const override final;

        virtual void SetData(const void* data, uint32_t size) override final;

    private:
        unsigned m_id;
    };

    // Currently supports 32-bit index buffers
    class SHINOBU_API OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer() override final;

        virtual void Bind() const override final;
        virtual void Unbind() const override final;

        virtual uint32_t GetCount() const override final;

    private:
        unsigned m_id;
        unsigned m_count;
    };

}