#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/VertexBuffer.h"

namespace sh
{
    class SHINOBU_API VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) = 0;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) = 0;
        
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

        static std::shared_ptr<VertexArray> Create();
    };
}