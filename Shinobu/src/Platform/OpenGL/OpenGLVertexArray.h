#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/VertexArray.h"

#include <vector>
#include <memory>

namespace sh
{
    class SHINOBU_API OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray() override final;

        virtual void Bind() override final;
        virtual void Unbind() override final;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) override final;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) override final;

        VertexBufferRef GetVertexBuffer(unsigned index) override;

        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const;

    private:
        std::vector<std::shared_ptr<VertexBuffer>> m_buffers;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
        unsigned m_id;
    };
}