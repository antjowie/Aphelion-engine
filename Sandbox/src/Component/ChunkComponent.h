#pragma once
#include "Block/BlockType.h"

#include <Shinobu/Core/Transform.h>
#include <Shinobu/Renderer/VertexArray.h>
#include <Shinobu/ECS/Registry.h>
#include <Shinobu/ECS/Serialize.h>

#include <glm/vec3.hpp>
#include <vector>

#include <bitsery/traits/array.h>

/// IVec so that we can use it easily in calculations
constexpr glm::ivec3 chunkDimensions {32,32,32};
constexpr unsigned chunkCount = chunkDimensions.x * chunkDimensions.y * chunkDimensions.z;
/// TODO: Instead of using default alloc, create a ChunkAlloc
//template <typename T>
//using ChunkSlice = std::vector<T>;
using ChunkContainer = std::vector<BlockType>;
    //ChunkSlice<ChunkSlice<ChunkSlice<BlockType,chunkDimensions.z>,chunkDimensions.y>, chunkDimensions.x>;
    //ChunkSlice<ChunkSlice<ChunkSlice<BlockType>>>;

struct ChunkDataComponent
{
    ChunkDataComponent()
        : pos(0)
    {
        chunk.resize(chunkCount);
        //chunk.resize(chunkDimensions.x);
        //for(int x = 0; x < chunkDimensions.x; x++)
        //{
        //    chunk[x].resize(chunkDimensions.y);
        //    for(int y = 0; y < chunkDimensions.y; y++)
        //    {
        //        chunk[x][y].resize(chunkDimensions.z);
        //    }
        //}
    }

    glm::vec3 pos;

    ChunkContainer chunk;

    bool operator==(const ChunkDataComponent& rhs) const
    {
        return true;
    }
};

template <typename S>
void serialize(S& s, ChunkDataComponent& o)
{
    //serialize(s,o.pos);
    //s.container1b(o.chunk);
}

struct ChunkMeshComponent
{
    sh::VertexArrayRef vao;

    bool operator==(const ChunkMeshComponent& rhs) const
    {
        return true;
    }
};

template <typename S>
void serialize(S& s, ChunkMeshComponent& o)
{
}

struct ChunkModifiedComponent
{
    char empty;

    bool operator==(const ChunkModifiedComponent& rhs) const
    {
        return true;
    }
};

template <typename S>
void serialize(S& s, ChunkModifiedComponent& o)
{
}

inline BlockType& GetBlock(ChunkContainer& chunk, unsigned x, unsigned y, unsigned z)
{
    return chunk.at((x * chunkDimensions.x * chunkDimensions.y) + (y * chunkDimensions.y) + z);
}

inline const BlockType& GetBlock(const ChunkContainer& chunk, unsigned x, unsigned y, unsigned z)
{
    return chunk.at((x * chunkDimensions.x * chunkDimensions.y) + (y * chunkDimensions.y) + z);
}

/**
 * Expected parameters
 * block& x y z
 */
template <typename Callable>
void ForEach(ChunkContainer& chunk, Callable& callable)
{
    for(auto x = 0; x < chunkDimensions.x; x++)
        for(auto y = 0; y < chunkDimensions.y; y++)
            for(auto z = 0; z < chunkDimensions.z; z++)
                callable(GetBlock(chunk,x,y,z),x,y,z);
                //callable(chunk[x][y][x],x,y,z);
}

template <typename Callable>
void ForEach(const ChunkContainer& chunk, Callable& callable)
{
    for(auto x = 0; x < chunkDimensions.x; x++)
        for(auto y = 0; y < chunkDimensions.y; y++)
            for(auto z = 0; z < chunkDimensions.z; z++)
                callable(GetBlock(chunk,x,y,z),x,y,z);
}

inline void RegisterChunkComponents()
{
    sh::Registry::RegisterComponent<ChunkDataComponent>();
    sh::Registry::RegisterComponent<ChunkMeshComponent>();
    sh::Registry::RegisterComponent<ChunkModifiedComponent>();
}