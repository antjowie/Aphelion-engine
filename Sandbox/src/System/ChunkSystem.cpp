#include "System/ChunkSystem.h"

void GenerateChunk(ChunkDataComponent& chunk)
{
    ForEach(chunk.chunk,[](BlockType& block, int x, int y, int z)
        {
            if (x % 2 == 0 && y % 2 == 0 && z % 2 == 0)
            {
                if (y < chunkDimensions.y / 3) block = BlockType::Grass;
                if (y > chunkDimensions.y / 3 * 2) block = BlockType::Dirt;
                else block = BlockType::Stone;
            }
            else
                block = BlockType::Air;

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
            if(y > 10)
                block = BlockType::Air;
            else if (y == chunkDimensions.y - 1)
                block = BlockType::Grass;
            else if (y > chunkDimensions.y - 4)
                block = BlockType::Dirt;
            else
                block = BlockType::Stone;
        });
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