#pragma once

#include "Aphelion/Core/Core.h"
#include "Aphelion/Renderer/VertexBuffer.h"

namespace ap
{
    class APHELION_API OpenGLVertexBuffer : public VertexBuffer
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
        virtual const std::vector<float>& GetData() const override final;

    private:
        uint32_t m_size; // NOTE: size is bytes, count is amount
        mutable std::vector<float> m_data;
        mutable bool m_dirty = true;

        unsigned m_id;
    };

    // Currently supports 32-bit index buffers
    class APHELION_API OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer() override final;

        virtual void Bind() const override final;
        virtual void Unbind() const override final;

        virtual const std::vector<uint32_t>& GetData() const override final;

        virtual uint32_t GetCount() const override final;

    private:
        uint32_t m_size;
        mutable std::vector<uint32_t> m_data;
        mutable bool m_dirty = true;

        unsigned m_id;
        unsigned m_count;
    };

}