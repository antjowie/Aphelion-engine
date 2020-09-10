#include "ChunkDataStructure.h"
#include "Block/BlockLibrary.h"
#include "Primitives.h"
#include <Aphelion/Renderer/Renderer.h>

void GenerateChunkMesh(ChunkData& chunk)
{
    auto& vao = chunk.vao;

    // Generate face chunk
    constexpr auto faceElemCount = faceAttributeCount * 4u;
    constexpr auto chunkElemCount = chunkCount * faceElemCount * 6u;

    // Essentially, this never gets deleted. But the size is always constant so it would be a waste to deallocate it
    // We can't allocate on the stack, it will cause an overflow. I think the limit is 1mb and we reach 6,75 with 32x32x32
    static auto chunkData = std::make_unique<float[]>(chunkElemCount);
    //static float* chunkData = new float[chunkElemCount];
    //float chunkData[chunkElemCount];

    unsigned elemIndex = 0;
    unsigned blockCount = 0;
    // Just generate every block in the chunk
    ForEach(chunk.data, [&](const BlockType& block, int x, int y, int z)
        {
            if (block == BlockType::Air) return;
            for (int i = 0; i < FaceDir::Count; i++)
            {

                auto vertices = GenerateFaceVertices(FaceDir(i), x, y, z, BlockLibrary::GetBlockData(block).texIndices[i]);
                memcpy_s(
                    &chunkData[elemIndex],
                    faceElemCount * sizeof(float),
                    vertices.data(),
                    faceElemCount * sizeof(float)
                );
                elemIndex += faceElemCount;
            }
            blockCount++;
        });

    // First time we generate this chunk
    if (!vao)
    {
        vao = ap::VertexArray::Create();

        auto vbo = ap::VertexBuffer::Create(chunkElemCount * sizeof(float));
        FillFaceVBOElements(vbo);

        vao->AddVertexBuffer(vbo);
    }

    auto vbo = vao->GetVertexBuffer(0);
    vbo->SetData(chunkData.get(), elemIndex * sizeof(float));

    vao->SetIndexBuffer(GenerateIndices(blockCount * 6u));
}

void GenerateChunkRb(ChunkData& chunk)
{
    // Generate rigid body
    auto& physics = chunk.rb;
    AP_ASSERT(!physics.Valid(), "Previous rb not deleted sucsessfully");
    //static unsigned nullGUID = 0;
    //physics.SetUserData(&nullGUID);

    // Calculate stride of vbo
    auto& vbo = chunk.vao->GetVertexBuffer(0);
    auto& elements = vbo->GetElements();
    unsigned stride = 0;
    for (const auto& elem : elements)
        stride += elem.size;

    auto material = ap::PhysicsMaterial(1.f, 1.f, 1.f);
    auto shape = ap::PhysicsShape(
        ap::PhysicsGeometry::CreateTriangleMesh(
            vbo->GetData(),
            chunk.vao->GetIndexBuffer()->GetData(),
            stride),
        material/*,
        glm::translate(glm::identity<glm::mat4>(), -chunk.pos)*/);
    physics = ap::RigidBody::CreateStatic(glm::translate(glm::identity<glm::mat4>(), glm::vec3(chunk.pos) /*+ (glm::vec3)chunkDimensions / 2.f*/));
    physics.AddShape(shape);
}


// Build mesh and physics if required
void BuildChunk(ChunkData& chunk)
{
    GenerateChunkMesh(chunk);
    GenerateChunkRb(chunk);
}

ChunkDataStructure::ChunkDataStructure(ap::PhysicsScene& scene)
    : m_shader(ap::Shader::Create("res/shader/Voxel.glsl"))
    , m_texture(ap::ArrayTexture2D::Create(2, 2, "res/texture.png"))
    , m_scene(scene)
{
    static const auto lightDir = glm::normalize(glm::vec3(0.1f, -1.f, -0.4f));
    m_shader->SetVec3("aLightDir", glm::value_ptr(lightDir));
    m_shader->SetFloat("aAmbient", 0.5f);
}

void ChunkDataStructure::AddChunk(const ChunkDataComponent& chunk)
{
    // Check if chunk already exists and is not received again
    if (m_chunks.count(chunk.pos) == 1)
    {
        auto& current = m_chunks.at(chunk.pos);
        if (current.chunkIter == chunk.chunkIter) return;
    }

    auto& data = m_chunks[chunk.pos];
    data.chunkIter = chunk.chunkIter;
    data.data = chunk.chunk;
    data.pos = chunk.pos;

    // Generate the chunk vao
    AP_TRACE("Building chunk {} {} {}", chunk.pos.x, chunk.pos.y, chunk.pos.z);

    if (data.rb.Valid())
        m_scene.RemoveActor(data.rb);

    if (chunk.isAir)
    {
        data.vao = nullptr;        
    }
    else
    {
        BuildChunk(data);
        m_scene.AddActor(data.rb);
    }
}

void ChunkDataStructure::Update()
{
}

void ChunkDataStructure::Render(const ap::PerspectiveCamera& cam)
{
    ap::Renderer::BeginScene(cam);
        
    m_texture->Bind();
    m_shader->Bind();

    for(const auto& chunk : m_chunks)
    {
        if (chunk.second.vao)
            ap::Renderer::Submit(m_shader, chunk.second.vao, glm::translate(glm::identity<glm::mat4>(), glm::vec3(chunk.first)));
    }

    ap::Renderer::EndScene();
}
