#pragma once

#include "Aphelion/Core/Core.h"
#include "Aphelion/Renderer/VertexArray.h"

#include <vector>
#include <memory>

namespace ap
{
    class APHELION_API OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray() override final;

        virtual void Bind() override final;
        virtual void Unbind() override final;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) override final;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) override final;

        VertexBufferRef GetVertexBuffer(unsigned index) override final;
        const VertexBufferRef& GetVertexBuffer(unsigned index) const override final;

        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override final;

    private:
        std::vector<std::shared_ptr<VertexBuffer>> m_buffers;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        unsigned m_id;
    };
}