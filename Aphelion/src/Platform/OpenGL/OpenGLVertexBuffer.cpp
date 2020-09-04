#include "Platform/OpenGL/OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace ap
{
    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
    {
        glCreateBuffers(1, &m_id);
        SetData(nullptr, size);
        //glNamedBufferData(m_id, size, nullptr, GL_STATIC_DRAW);
        //glNamedBufferData(m_id, size, nullptr, GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    }
    
    OpenGLVertexBuffer::OpenGLVertexBuffer(const float* vertices, uint32_t size)
    {
        glCreateBuffers(1, &m_id);
        SetData(vertices, size);
        //Bind();
        //glNamedBufferData(m_id, size, vertices, GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }
    
    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        glDeleteVertexArrays(1, &m_id);
    }

    void OpenGLVertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
    }
    
    void OpenGLVertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
    {
        //Bind();
        /*
        https://www.khronos.org/opengl/wiki/Buffer_Object

        Initially I was worried that the buffer type is not specified (glBufferData sets up internal state)
        According to the wiki however, while filling the buffer the target doesn't matter
        */
        glNamedBufferData(m_id, size, data, GL_STATIC_DRAW);
        m_dirty = true;
        m_size = size;
    }

    const std::vector<float>& OpenGLVertexBuffer::GetData() const
    {
        if (m_dirty)
        {
            m_dirty = false;
            m_data.clear();
            // We have to use resize or else the vector size var doesn't get updated
            m_data.resize(m_size / sizeof(float));

            // Get data from GPU
            glGetNamedBufferSubData(m_id, 0, m_size, m_data.data());
        }

        return m_data;
    }

    ///////////////////////////////////
    // Vertex buffer
    ///////////////////////////////////
    OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count)
        : m_count(count)
    {
        //glGenBuffers(1, &m_id);
        glCreateBuffers(1, &m_id);
        //Bind();
        m_size = sizeof(uint32_t) * count;
        glNamedBufferData(m_id, m_size, indices, GL_STATIC_DRAW);
        m_dirty = true;
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        glDeleteBuffers(1, &m_id);
    }

    void OpenGLIndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    }
    
    void OpenGLIndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    }

    const std::vector<uint32_t>& OpenGLIndexBuffer::GetData() const
    {
        if (m_dirty)
        {
            m_dirty = false;
            m_data.clear();
            m_data.resize(m_size / sizeof(uint32_t));

            // Get data from GPU
            glGetNamedBufferSubData(m_id, 0, m_size, m_data.data());
        }

        return m_data;
    }

    uint32_t OpenGLIndexBuffer::GetCount() const
    {
        return m_count;
    }
}