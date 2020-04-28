#include "Platform/OpenGL/OpenGLVertexArray.h"

#include <glad/glad.h>

namespace sh
{
    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_id);
    }
    
    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_id);
    }

    void OpenGLVertexArray::Bind()
    {
        glBindVertexArray(m_id);
    }
   
    void OpenGLVertexArray::Unbind()
    {
        glBindVertexArray(0);
    }
    
    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer)
    {
        Bind();
        buffer->Bind();
        for (auto elem : buffer->GetElements())
        {
            glEnableVertexAttribArray(elem.index);
            glVertexAttribPointer(
                elem.index,
                elem.count,
                elem.dataType,
                elem.normalized,
                elem.stride,
                (void*)elem.offset
            );
        }

        m_buffers.push_back(buffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer)
    {
        Bind();
        buffer->Bind();

        m_indexBuffer = buffer;
    }

    const std::shared_ptr<IndexBuffer>& OpenGLVertexArray::GetIndexBuffer() const
    {
        return m_indexBuffer;
    }
}
