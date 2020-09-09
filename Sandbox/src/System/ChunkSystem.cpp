#include "System/ChunkSystem.h"

void GenerateChunk(ChunkDataComponent& chunk)
{
    auto pos = chunk.pos;
    bool isSolid = false;
    ForEach(chunk.chunk,[&](BlockType& block, int x, int y, int z)
        {
            //if (x % 2 == 0 && y % 2 == 0 && z % 2 == 0)
            //{
            //    if (y < chunkDimensions.y / 3) block = BlockType::Grass;
            //    if (y > chunkDimensions.y / 3 * 2) block = BlockType::Dirt;
            //    else block = BlockType::Stone;
            //}
            //else
            //    block = BlockType::Air;

            //return;

            //if (x == 0 && z == 0)
            //{
            //    if (y == 4)
            //        block = BlockType::Grass;

            //    if (y == 2)
            //        block = BlockType::Dirt;

            //    if (y == 0)
            //        block = BlockType::Stone;
            //}
            //else
            //    block = BlockType::Air;
            //return;

            // TODO: Refactor to chunk strategy
            x += pos.x;
            y += pos.y;
            z += pos.z;

            if (y <= 6)
                isSolid = true;

            if (y > 6)
                block = BlockType::Air;
            else if (y == 6)
                block = BlockType::Grass;
            else if (y > 2)
                block = BlockType::Dirt;
            else
                block = BlockType::Stone;
        });

    chunk.isAir = !isSolid;
}

void GenerateChunkMesh(const ChunkDataComponent& chunk, ap::VertexArrayRef& vao)
{
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
    ForEach(chunk.chunk,[&](const BlockType& block, int x, int y, int z)
        {
            if(block == BlockType::Air) return;
            for(int i = 0; i < FaceDir::Count; i++)
            {

                auto vertices = GenerateFaceVertices(FaceDir(i),x,y,z,BlockLibrary::GetBlockData(block).texIndices[i]);
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
    if(!vao)
    {
        vao = ap::VertexArray::Create();

        auto vbo = ap::VertexBuffer::Create(chunkElemCount * sizeof(float));
        FillFaceVBOElements(vbo);

        vao->AddVertexBuffer(vbo);
    }

    auto vbo = vao->GetVertexBuffer(0);
    vbo->SetData(chunkData.get(),elemIndex * sizeof(float));

    vao->SetIndexBuffer(GenerateIndices(blockCount*6u));
}

float ChunkStrategy::m_radius{ 4 };
ap::Scene* ChunkStrategy::m_scene;
std::unordered_map<glm::ivec3, ChunkStrategy::ChunkProxy> ChunkStrategy::m_chunks;
std::list<std::reference_wrapper<ChunkStrategy::ChunkProxy>> ChunkStrategy::m_chunksNotFilled;

void ChunkStrategy::AddChunk(ap::Entity& chunk)
{
    ////////////////////////////////////////////////////
    // Check adjacent chunks for neighbors
    //   If adjacent chunk full neighbors 
    //     generate mesh for adjacent chunk
    //     Remove from chunkNotFilled
    ////////////////////////////////////////////////////
    auto& chunkData = chunk.GetComponent<ChunkDataComponent>();
    auto chunkWorldPos = chunkData.pos;
    auto& current = m_chunks[chunkWorldPos];
    current.chunkGUID = chunk.GetComponent<ap::GUIDComponent>();

    auto ecsCount = m_scene->GetRegistry().Count<ChunkDataComponent>();
    auto stratCount = m_chunks.size();
    AP_ASSERT(stratCount == ecsCount, "ECS and Strategy are out of sync");
    AP_ASSERT(chunkWorldPos % chunkDimensions == glm::ivec3{0}, "Chunk pos not in chunk coords");

    AP_INFO("Got chunk {} {} {}", chunkWorldPos.x, chunkWorldPos.y, chunkWorldPos.z);

    // Check adjacent chunks for neighbors
    for (int i = 0; i < 6; i++)
    {
        auto* dir = current.neighbor.v[i];
        if (!dir)
        {
            // Calculate neighbor position
            //Chunk* top;
            //Chunk* left;
            //Chunk* front;
            //Chunk* bottom;
            //Chunk* right;
            //Chunk* back;
            glm::ivec3 offset(0);
            if (i == 0) offset.y = (int)chunkDimensions.y;
            else if (i == 1) offset.x = -(int)chunkDimensions.x;
            else if (i == 2) offset.z = (int)chunkDimensions.z;
            else if (i == 3) offset.y = -(int)chunkDimensions.y;
            else if (i == 4) offset.x = (int)chunkDimensions.x;
            else if (i == 5) offset.z = -(int)chunkDimensions.z;
            glm::ivec3 neighborPos = chunkWorldPos + offset;

            if (m_chunks.count(neighborPos) == 1)
            {
                auto& neighbor = m_chunks.at(neighborPos);

                current.neighbor.v[i] = &neighbor;
                current.neighborCount++;

                neighbor.neighbor.v[(i + 3) % 6] = &current;
                neighbor.neighborCount++;

                // If adjacent chunk full neighbors 
                if (neighbor.neighborCount == 6)
                {
                    m_scene->GetRegistry().Get(neighbor.chunkGUID).GetComponent<ChunkModifiedComponent>().force = true;
                    m_chunksNotFilled.remove(neighbor);
                }
            }
        }
    }

    ////////////////////////////////////////////////////
    // If no neighbor 
    //   generate own mesh
    //   Add to chunkNotFilled
    ////////////////////////////////////////////////////
    chunk.GetComponent<ChunkModifiedComponent>().force = true;
    if (current.neighborCount != 6)
        m_chunksNotFilled.push_back(current);
}

void ChunkStrategy::RemoveChunk(ap::Entity& chunk)
{
    ////////////////////////////////////////////////////
    // Remove from neighbors
    //   If neighbor was full
    //     generate mesh for neighbor
    //     Add to chunkNotFilled
    ////////////////////////////////////////////////////
    auto& reg = m_scene->GetRegistry();
    auto& chunkData = chunk.GetComponent<ChunkDataComponent>();
    auto chunkWorldPos = chunkData.pos;
    auto& current = m_chunks[chunkWorldPos];
    current.chunkGUID = chunk.GetComponent<ap::GUIDComponent>();

    for (int i = 0; i < 6; i++)
    {
        auto* dir = current.neighbor.v[i];
        if (dir)
        {
            auto& neighbor = *dir;

            // Redundant but nice for verification
            current.neighbor.v[i] = nullptr;
            current.neighborCount--;

            neighbor.neighbor.v[(i + 3) % 6] = nullptr;
            neighbor.neighborCount--;

            // If neighbor was full
            if (neighbor.neighborCount == 5)
            {
                auto nE = reg.Get(neighbor.chunkGUID);
                if (!nE.HasComponent<ChunkModifiedComponent>()) nE.AddComponent<ChunkModifiedComponent>();
                nE.GetComponent<ChunkModifiedComponent>().force = true;
                m_chunksNotFilled.push_back(neighbor);
            }
        }
    }

    reg.Destroy(chunk);

    // Remove from chunkNotFilled
    m_chunksNotFilled.remove(current);
}

void ChunkStrategy::operator()(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();

    // Check if we have center chunk
    glm::ivec3 playerPos(-1);
    reg.View<ap::TransformComponent, Player>(
        [&](ap::Entity& e, ap::TransformComponent& t, Player&)
        {
            playerPos = glm::ivec3(t.t.GetPosition());
        });
    //AP_INFO(playerPos.x);
    glm::ivec3 playerChunkPos = playerPos - (playerPos % chunkDimensions);
    playerChunkPos /= chunkDimensions;
    if (playerPos != glm::ivec3(-1) && m_chunks.count(playerChunkPos) == 0)
    {
        ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(ap::Serialize(ChunkSpawnComponent{playerChunkPos * chunkDimensions}, 0)));
        return;
    }

    ////////////////////////////////////////////////////
    // For chunks
    //   If out of radius
    //     Remove from neighbors
    //     If neighbors no longer full
    //       generate mesh of neighbor
    ////////////////////////////////////////////////////
    for (auto& chunk : m_chunks)
    {
        glm::ivec3 chunkPos = chunk.first / chunkDimensions;
        const int distance = glm::length(glm::vec3(playerChunkPos - chunkPos));

        if (distance > m_radius)
        {
            RemoveChunk(reg.Get(chunk.second.chunkGUID));
        }
    }

    ////////////////////////////////////////////////////
    // For chunkNotFilled
    // For empty neighbors 
    //   If in radius 
    //     request chunk
    ////////////////////////////////////////////////////
    int chunkRequestCount = 6;
    for (auto& chunk : m_chunksNotFilled)
    {
        auto& current = chunk.get();
        // For empty neighbors 
        for (int i = 0; i < 6; i++)
        {
            auto* dir = current.neighbor.v[i];
            if (!dir)
            {
                // Calculate neighbor position
                //Chunk* top;
                //Chunk* left;
                //Chunk* front;
                //Chunk* bottom;
                //Chunk* right;
                //Chunk* back;
                glm::ivec3 offset(0);
                if (i == 0) offset.y = (int)chunkDimensions.y;
                else if (i == 1) offset.x = -(int)chunkDimensions.x;
                else if (i == 2) offset.z = (int)chunkDimensions.z;
                else if (i == 3) offset.y = -(int)chunkDimensions.y;
                else if (i == 4) offset.x = (int)chunkDimensions.x;
                else if (i == 5) offset.z = -(int)chunkDimensions.z;
                glm::ivec3 neighborPos = reg.Get(current.chunkGUID).GetComponent<ChunkDataComponent>().pos + offset;

                ChunkSpawnComponent spawn;
                spawn.pos = neighborPos;
                ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(ap::Serialize(spawn, 0)));
                //AP_TRACE("Requesting {} {} {}", spawn.pos.x, spawn.pos.y, spawn.pos.z);
                chunkRequestCount--;
                if (chunkRequestCount == 0) return;

                //if (m_chunks.count(neighborPos) == 1)
                //{
                //    auto& neighbor = m_chunks.at(neighborPos);

                //    current.neighbor.v[i] = &neighbor;
                //    current.neighborCount++;

                //    neighbor.neighbor.v[(i + 3) % 6] = &current;
                //    neighbor.neighborCount++;

                //    // If adjacent chunk full neighbors 
                //    if (neighbor.neighborCount == 6)
                //    {
                //        m_scene->GetRegistry().Get(neighbor.chunkGUID).AddComponent<ChunkModifiedComponent>().force = true;
                //        m_chunksNotFilled.remove(neighbor);
                //    }
                //}
            }
        }
    }
}
