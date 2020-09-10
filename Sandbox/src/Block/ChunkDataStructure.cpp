#include "ChunkDataStructure.h"
#include "Block/BlockLibrary.h"
#include "Primitives.h"
#include <Aphelion/Renderer/Renderer.h>
#include <Aphelion/Core/Application.h>
#include <Aphelion/Core/Event/NetEvent.h>


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
    auto& rb = chunk.rb;
    rb = ap::RigidBody::CreateStatic(glm::translate(glm::identity<glm::mat4>(), glm::vec3(chunk.pos) /*+ (glm::vec3)chunkDimensions / 2.f*/));
    rb.AddShape(ap::PhysicsShape{ ap::PhysicsGeometry::CreateBox(chunkDimensions/2), ap::PhysicsMaterial(1.f, 1.f, 1.f) });

    return;

    AP_ASSERT(!rb.Valid(), "Previous rb not deleted sucsessfully");
    //static unsigned nullGUID = 0;
    //rb.SetUserData(&nullGUID);

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
    rb = ap::RigidBody::CreateStatic(glm::translate(glm::identity<glm::mat4>(), glm::vec3(chunk.pos) /*+ (glm::vec3)chunkDimensions / 2.f*/));
    rb.AddShape(shape);
}


// Build mesh and physics if required
void BuildChunk(ChunkData& chunk)
{
    GenerateChunkMesh(chunk);
    GenerateChunkRb(chunk);
}

glm::ivec3 GetNeighborWorldPos(const glm::ivec3& chunkWorldPos, int dir)
{
    glm::ivec3 offset(0);
    if (dir == 0) offset.y = (int)chunkDimensions.y;
    else if (dir == 1) offset.x = -(int)chunkDimensions.x;
    else if (dir == 2) offset.z = (int)chunkDimensions.z;
    else if (dir == 3) offset.y = -(int)chunkDimensions.y;
    else if (dir == 4) offset.x = (int)chunkDimensions.x;
    else if (dir == 5) offset.z = -(int)chunkDimensions.z;

    return chunkWorldPos + offset;
}

bool FullyPopulated(const std::array<ChunkData*, 6>& neighbors)
{
    for (auto n : neighbors)
        if (!n) return false;
    return true;
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
    //AP_INFO("Got {} {} {}", chunk.pos.x, chunk.pos.y, chunk.pos.z);
    // Check if chunk already exists and is not received again
    if (m_chunks.count(chunk.pos) == 1)
    {
        // Remove from chunks to request
        auto request = std::find(m_chunksToRequest.begin(), m_chunksToRequest.end(), chunk.pos);
        if (request != m_chunksToRequest.end()) m_chunksToRequest.erase(request);

        auto& current = m_chunks.at(chunk.pos);
        if (current.chunkIter == chunk.chunkIter) return;
    }

    auto& data = m_chunks[chunk.pos];
    data.chunkIter = chunk.chunkIter;
    data.data = chunk.chunk;
    data.pos = chunk.pos;

    // Generate the chunk vao
    //AP_TRACE("Building chunk {} {} {}", chunk.pos.x, chunk.pos.y, chunk.pos.z);

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

    // Remove from chunks to request
    auto request = std::find(m_chunksToRequest.begin(), m_chunksToRequest.end(), data.pos);
    if (request != m_chunksToRequest.end()) m_chunksToRequest.erase(request);

    // If inside radius, request neighbor chunks
    const auto distance = glm::length2(glm::vec3(data.pos - m_playerChunkPos) / glm::vec3(chunkDimensions));
    if (distance < m_radius * m_radius)
    {
        auto neighbors = GetNeighbors(data.pos);

        for (int i = 0; i < 6; i++)
        {
            auto& n = neighbors[i];
            if (!n)
            {
                if (chunk.isSolid && i == Dir::Bottom) break;
                // NOTE: this is gonna make it so that blocks in the air are not spawned
                // It's gonna look pretty weird
                if (chunk.isAir && i != Dir::Bottom) break;
                
                auto nPos = GetNeighborWorldPos(data.pos, i);
                //AP_INFO("Requesting {} {} {}", nPos.x, nPos.y, nPos.z);
                m_chunksToRequest.push_back(nPos);                
            }
        }
    }
}

void ChunkDataStructure::Update(float dt)
{
    /*
    How to implement prioritized chunk loading
    I will just load in a circular pattern. It seems the easiest for now. No prio at all
    In a fifo queue load the initial chunk. 
    For each chunk without full neighbors
        If in render distance
        Add all neighboring chunks to the list.

    Do the following every t seconds.
    Check if we can remove chunks that are outside of radius.
    Check if center chunk still has full neighbors
        else sort non full neighbors based on distance from center
    Get the chunks without fully populated neighbors and request their chunks if inside radius.
    */

    // Request 10 times a second
    static float cooldown = 0.f;
    if (cooldown <= 0.f)
        cooldown = 0.05f;
    else
    {
        cooldown -= dt;
        return;
    }

    if (m_chunks.count(m_playerChunkPos) == 0)
    {
        ChunkSpawnComponent spawn;
        spawn.pos = m_playerChunkPos;
        ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(ap::Serialize(spawn, 0)));
        return;
    }

    //AP_TRACE(m_chunksToRequest.size());

    int iter = 6;
    for (const auto& request : m_chunksToRequest)
    {
        //AP_WARN("Waiting for {} {} {}", request.x, request.y, request.z);

        ChunkSpawnComponent spawn;
        spawn.pos = request;
        ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(ap::Serialize(spawn, 0)));

        iter--;
        if (iter <= 0) return;
    }
}

void ChunkDataStructure::SetPos(const glm::vec3& playerPos)
{
    auto oldPos = m_playerChunkPos;
    m_playerChunkPos = WorldToChunkCoordinates(glm::ivec3(playerPos));

    if (m_playerChunkPos != oldPos)
    {
        // TODO: Resort request queue
        //m_chunksToRequest.clear();
        std::sort(m_chunksToRequest.begin(), m_chunksToRequest.end(), [&](const glm::ivec3& lhs, const glm::ivec3& rhs)
            {
                auto dis1 = glm::distance2(glm::vec3(lhs), glm::vec3(m_playerChunkPos));
                auto dis2 = glm::distance2(glm::vec3(lhs), glm::vec3(m_playerChunkPos));
                return dis1 < dis2;
            });
    }
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

std::array<ChunkData*, 6> ChunkDataStructure::GetNeighbors(const glm::ivec3& chunkWorldPos)
{
    std::array<ChunkData*, 6> ret;
   
    for (int i = 0; i < 6; i++)
    {
        auto neighborPos = GetNeighborWorldPos(chunkWorldPos, i);

        ret[i] = nullptr;
        if (m_chunks.count(neighborPos) == 1)
            ret[i] = &m_chunks.at(neighborPos);
    }

    return ret;
}
