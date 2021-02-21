#pragma once
#include <array>
#include <Aphelion/Renderer/VertexBuffer.h>

#if 0
namespace Vertices
{
    constexpr float planeVertices[]
    {
        -1.f, 1.f, 0.f, 0.f, 1.f,
        -1.f, -1.f, 0.f, 0.f, 0.f,
        1.f, -1.f, 0.f, 1.f, 0.f,
        1.f, 1.f, 0.f, 1.f, 1.f,
    };
    constexpr unsigned planeIndices[]
    {
        0, 1, 3,
        1, 2, 3,
    };
    constexpr float cubeVertices[]
    {
        // Front face
        -1.0, -1.0, 1.0, 0.f, 0.f,
        1.0, -1.0, 1.0, 1.f, 0.f,
        1.0, 1.0, 1.0, 1.f, 1.f,
        -1.0, 1.0, 1.0, 0.f, 1.f,

        // Back face
        -1.0, -1.0, -1.0, 1.f, 0.f,
        -1.0, 1.0, -1.0, 1.f, 1.f,
        1.0, 1.0, -1.0, 0.f, 1.f,
        1.0, -1.0, -1.0, 0.f, 0.f,

        // Top face
        -1.0, 1.0, -1.0, 0.f, 1.f,
        -1.0, 1.0, 1.0, 0.f, 0.f,
        1.0, 1.0, 1.0, 1.f, 0.f,
        1.0, 1.0, -1.0, 1.f, 1.f,

        // Bottom face
        -1.0, -1.0, -1.0, 0.f, 0.f,
        1.0, -1.0, -1.0, 1.f, 0.f,
        1.0, -1.0, 1.0, 1.f, 1.f,
        -1.0, -1.0, 1.0, 0.f, 1.f,

        // Right face
        1.0, -1.0, -1.0, 1.f, 0.f,
        1.0, 1.0, -1.0, 1.f, 1.f,
        1.0, 1.0, 1.0, 0.f, 1.f,
        1.0, -1.0, 1.0, 0.f, 0.f,

        // Left face
        -1.0, -1.0, -1.0, 0.f, 0.f,
        -1.0, -1.0, 1.0, 1.f, 0.f,
        -1.0, 1.0, 1.0, 1.f, 1.f,
        -1.0, 1.0, -1.0, 0.f, 1.f,
    };
    constexpr uint32_t cubeIndices[]{
        0, 1, 2, 0, 2, 3, // front
        4, 5, 6, 4, 6, 7, // back
        8, 9, 10, 8, 10, 11, // top
        12, 13, 14, 12, 14, 15, // bottom
        16, 17, 18, 16, 18, 19, // right
        20, 21, 22, 20, 22, 23, // left
    };
}
#endif

// pos3 nor3 tex2 texIndex1
constexpr unsigned faceAttributeCount {3 + 3 + 2 + 1};
using FaceVertices = std::array<float, 4 * faceAttributeCount>;
using FaceIndices = std::array<unsigned, 6>;
using CubeVertices = std::array<float, 8 * faceAttributeCount>;
using CubeIndices = std::array<unsigned, 3 * 2 * 6>;

/// We assume left handed coordinate system
enum FaceDir
{
    Top,
    Bottom,
    Left,
    Right,
    Back,
    Front,

    Count
};

FaceVertices GenerateFaceVertices(FaceDir dir, float x, float y, float z, float texIndex);
inline void FillFaceVBOElements(ap::VertexBufferRef& buffer)
{
    buffer->AddElement({ap::ShaderDataType::Float3, "aPos"});
    buffer->AddElement({ap::ShaderDataType::Float3, "aNormal",true});
    buffer->AddElement({ap::ShaderDataType::Float2, "aTex",true});
    buffer->AddElement({ap::ShaderDataType::Float, "aIndex"});
}

template <unsigned faceCount>
ap::IndexBufferRef GenerateIndices()
{
    constexpr auto elementCount = faceCount * 6u;

    std::array<unsigned,elementCount> ebo;
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

    return ap::IndexBuffer::Create(ebo.data(),ebo.size());
}

/// The runtime variant
inline ap::IndexBufferRef GenerateIndices(unsigned faceCount)
{
    auto elementCount = faceCount * 6u;

    std::vector<unsigned> ebo;
    ebo.resize(elementCount);
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

    return ap::IndexBuffer::Create(ebo.data(),ebo.size());
}
