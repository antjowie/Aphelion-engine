#pragma once

#include "Aphelion/Core/Core.h"
#include "Aphelion/Renderer/VertexBuffer.h"

namespace ap
{
    class APHELION_API VertexArray;
    using VertexArrayRef = std::shared_ptr<VertexArray>;

    class APHELION_API VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) = 0;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) = 0;

        virtual VertexBufferRef GetVertexBuffer(unsigned index) = 0;
        virtual const VertexBufferRef& GetVertexBuffer(unsigned index) const = 0;

        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

        static VertexArrayRef Create();
    };
}