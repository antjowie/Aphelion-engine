#pragma once

#include "Shinobu/Core/Core.h"

#include <vector>

namespace sh
{
    class SHINOBU_API VertexBuffer
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
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual void AddElement(const Element& elem);
        virtual const std::vector<Element>& GetElements() const;

        static std::shared_ptr<VertexBuffer> Create(uint32_t size);
        static std::shared_ptr<VertexBuffer> Create(const float* vertices, uint32_t size);

    private:
        std::vector<Element> m_elements;
    };

    // Currently supports 32-bit index buffers
    class SHINOBU_API IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetCount() const = 0;

        static std::shared_ptr<IndexBuffer> Create(const uint32_t* indices, uint32_t count);
    };

}