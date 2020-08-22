#include "Primitive.h"

namespace ap
{
    void SetVBOElements(ap::VertexBufferRef& buffer)
    {
        buffer->AddElement({ ap::ShaderDataType::Float3, "aPos" });
        buffer->AddElement({ ap::ShaderDataType::Float2, "aTex", true });
    }


    // pos3, tex2
    constexpr float cubeVertices[]
    {
        // Front face
        -0.5f, -0.5f, 0.5f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f, 1.f, 1.f,
        -0.5f, 0.5f, 0.5f, 0.f, 1.f,

        // Back face
        -0.5f, -0.5f, -0.5f, 1.f, 0.f,
        -0.5f, 0.5f, -0.5f, 1.f, 1.f,
        0.5f, 0.5f, -0.5f, 0.f, 1.f,
        0.5f, -0.5f, -0.5f, 0.f, 0.f,

        // Top face
        -0.5f, 0.5f, -0.5f, 0.f, 1.f,
        -0.5f, 0.5f, 0.5f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f, 1.f, 0.f,
        0.5f, 0.5f, -0.5f, 1.f, 1.f,

        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.f, 0.f,
        0.5f, -0.5f, 0.5f, 1.f, 1.f,
        -0.5f, -0.5f, 0.5f, 0.f, 1.f,

        // Right face
        0.5f, -0.5f, -0.5f, 1.f, 0.f,
        0.5f, 0.5f, -0.5f, 1.f, 1.f,
        0.5f, 0.5f, 0.5f, 0.f, 1.f,
        0.5f, -0.5f, 0.5f, 0.f, 0.f,

        // Left face
        -0.5f, -0.5f, -0.5f, 0.f, 0.f,
        -0.5f, -0.5f, 0.5f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f, 1.f, 1.f,
        -0.5f, 0.5f, -0.5f, 0.f, 1.f,
    };

    constexpr uint32_t cubeIndices[]{
        0, 1, 2, 0, 2, 3, // front
        4, 5, 6, 4, 6, 7, // back
        8, 9, 10, 8, 10, 11, // top
        12, 13, 14, 12, 14, 15, // bottom
        16, 17, 18, 16, 18, 19, // right
        20, 21, 22, 20, 22, 23, // left
    };

    VertexArrayRef CreateCube()
    {
        auto vao = VertexArray::Create();
        auto vbo = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
        SetVBOElements(vbo);

        auto ibo = IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(cubeIndices[0]));
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ibo);
        
        return vao;
    }
    
    VertexArrayRef CreatePlane()
    {
        return VertexArray::Create();
    }
}