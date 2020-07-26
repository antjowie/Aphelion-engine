#pragma once
#include "Block/BlockType.h"

#include <Shinobu/Core/Transform.h>
#include <Shinobu/Renderer/VertexArray.h>
#include <Shinobu/ECS/Registry.h>
#include <Shinobu/ECS/Serialize.h>

#include <glm/vec3.hpp>

#include <bitsery/traits/array.h>


/// IVec so that we can use it easily in calculations
constexpr glm::ivec3 chunkDimensions {4,32,4};
constexpr unsigned chunkCount = chunkDimensions.x * chunkDimensions.y * chunkDimensions.z;
/**
 * TODO: We probably want to create a memory pool for the chunks
 * Unless chunks are not too big.
 */
template <typename T, unsigned size>
using ChunkSlice = std::array<T, size>;
using ChunkArray =
    ChunkSlice<ChunkSlice<ChunkSlice<BlockType,chunkDimensions.z>,chunkDimensions.y>, chunkDimensions.x>;

/**
 * Expected parameters
 * block& x y z
 */
template <typename Callable>
void ForEach(ChunkArray& chunk, Callable& callable)
{
    for(auto x = 0; x < chunkDimensions.x; x++)
        for(auto y = 0; y < chunkDimensions.y; y++)
            for(auto z = 0; z < chunkDimensions.z; z++)
                callable(chunk[x][y][x],x,y,z);
}

template <typename Callable>
void ForEach(const ChunkArray& chunk, Callable& callable)
{
    for(auto x = 0; x < chunkDimensions.x; x++)
        for(auto y = 0; y < chunkDimensions.y; y++)
            for(auto z = 0; z < chunkDimensions.z; z++)
                callable(chunk[x][y][x],x,y,z);
}

struct ChunkDataComponent
{
    glm::vec3 pos;

    ChunkArray chunk;

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

inline void RegisterChunkComponents()
{
    sh::Registry::RegisterComponent<ChunkDataComponent>();
    sh::Registry::RegisterComponent<ChunkMeshComponent>();
    sh::Registry::RegisterComponent<ChunkModifiedComponent>();
}