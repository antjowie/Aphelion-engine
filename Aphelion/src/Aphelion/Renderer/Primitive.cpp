#include "Primitive.h"

namespace ap
{
    void SetVBOElements(ap::VertexBufferRef& buffer)
    {
        buffer->AddElement({ ap::ShaderDataType::Float3, "aPos" });
        buffer->AddElement({ ap::ShaderDataType::Float3, "aNormal"});
        buffer->AddElement({ ap::ShaderDataType::Float2, "aTex", true });
    }


    // pos3,  nor3, tex2
    constexpr float cubeVertices[]
    {
        // top
        -0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f, 1.f,
        -0.5f, 0.5f,  0.5f, 0.f, 1.f, 0.f, 0.f, 0.f,
         0.5f, 0.5f,  0.5f, 0.f, 1.f, 0.f, 1.f, 0.f,
         0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f, 1.f, 1.f,

        // bottom
         0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f, 1.f, 1.f,
         0.5f, -0.5f,  0.5f, 0.f, -1.f, 0.f, 1.f, 0.f,
        -0.5f, -0.5f,  0.5f, 0.f, -1.f, 0.f, 0.f, 0.f,
        -0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f, 0.f, 1.f,

        // left
        -0.5f,  0.5f, -0.5f, -1.f, 0.f, 0.f, 1.f, 1.f,
        -0.5f, -0.5f, -0.5f, -1.f, 0.f, 0.f, 1.f, 0.f,
        -0.5f, -0.5f,  0.5f, -1.f, 0.f, 0.f, 0.f, 0.f,
        -0.5f,  0.5f,  0.5f, -1.f, 0.f, 0.f, 0.f, 1.f,

        // right
        0.5f,  0.5f,  0.5f, 1.f, 0.f, 0.f, 0.f, 1.f,
        0.5f, -0.5f,  0.5f, 1.f, 0.f, 0.f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f, 0.f,
        0.5f,  0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f, 1.f,

        // back
         0.5f,  0.5f, -0.5f, 0.f, 0.f, -1.f, 1.f, 1.f,
         0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f, 1.f, 0.f,
        -0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f, 0.f, 0.f,
        -0.5f,  0.5f, -0.5f, 0.f, 0.f, -1.f, 0.f, 1.f,
        
        // front
        -0.5f,  0.5f, 0.5f, 0.f, 0.f, 1.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f, 0.f, 0.f,
         0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f, 1.f, 0.f,
         0.5f,  0.5f, 0.5f, 0.f, 0.f, 1.f, 1.f, 1.f,
    };

    template <unsigned faceCount>
    ap::IndexBufferRef GenerateIndices()
    {
        constexpr auto elementCount = faceCount * 6u;

        std::array<unsigned, elementCount> ebo;
        unsigned offset = 0;
        for (unsigned i = 0; i < elementCount; i += 6)
        {
            ebo[i + 0] = 3 + offset;
            ebo[i + 1] = 0 + offset;
            ebo[i + 2] = 1 + offset;
            ebo[i + 3] = 3 + offset;
            ebo[i + 4] = 1 + offset;
            ebo[i + 5] = 2 + offset;

            //std::vector<unsigned> indices { 3 + offset,0 + offset,1 + offset,3 + offset,1 + offset,2 + offset };
            //ebo.insert(ebo.end(), indices.begin(), indices.end());
            //memcpy(&ebo[i], indices, 6 * sizeof(unsigned));
            offset += 4;
        }

        return ap::IndexBuffer::Create(ebo.data(), ebo.size());
    }

    VertexArrayRef CreateCube()
    {
        auto vao = VertexArray::Create();
        auto vbo = VertexBuffer::Create(cubeVertices, sizeof(cubeVertices));
        SetVBOElements(vbo);

        auto ibo = GenerateIndices<6>();
        vao->AddVertexBuffer(vbo);
        vao->SetIndexBuffer(ibo);
        
        return vao;
    }
    
    VertexArrayRef CreatePlane()
    {
        return VertexArray::Create();
    }
}