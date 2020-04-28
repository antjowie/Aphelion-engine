#pragma once

#include "Shinobu/Core/Core.h"

#include <vector>
#include <string>

namespace sh
{
	enum class ShaderDataType
	{
		None = 0, 
		Float, 
		Float2, 
		Float3, 
		Float4, 
		Mat3, 
		Mat4, 
		Int, 
		Int2, 
		Int3, 
		Int4, 
		Bool
	};

	uint32_t ShaderDataTypeSize(ShaderDataType type);

	struct BufferElement
	{	
        BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: name(name)
			, type(type)
			, size(ShaderDataTypeSize(type))
			, offset(0)
			, normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const;
		
		std::string name;
		ShaderDataType type;
		uint32_t size;
		size_t offset;
		bool normalized;
	};

    class SHINOBU_API VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual void AddElement(const BufferElement& elem);
        virtual const std::vector<BufferElement>& GetElements() const;

        static std::shared_ptr<VertexBuffer> Create(uint32_t size);
        static std::shared_ptr<VertexBuffer> Create(const float* vertices, uint32_t size);

    private:
        std::vector<BufferElement> m_elements;
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