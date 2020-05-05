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

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}

		SH_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

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

	class SHINOBU_API VertexBuffer;
	using VertexBufferRef = std::shared_ptr<VertexBuffer>;

    class SHINOBU_API VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual void AddElement(const BufferElement& elem);
        virtual const std::vector<BufferElement>& GetElements() const;

        static VertexBufferRef Create(uint32_t size);
        static VertexBufferRef Create(const float* vertices, uint32_t size);

    private:
        std::vector<BufferElement> m_elements;
    };

	class SHINOBU_API IndexBuffer;
	using IndexBufferRef = std::shared_ptr<IndexBuffer>;

    // Currently supports 32-bit index buffers
    class SHINOBU_API IndexBuffer
    {
    public:
        virtual ~IndexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetCount() const = 0;

        static IndexBufferRef Create(const uint32_t* indices, uint32_t count);
    };
}