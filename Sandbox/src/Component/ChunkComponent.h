#pragma once
#include "Block/BlockType.h"

#include <Aphelion/Core/Transform.h>
#include <Aphelion/Renderer/VertexArray.h>
#include <Aphelion/ECS/Registry.h>

#include <glm/vec3.hpp>
#include <vector>

#include <bitsery/traits/array.h>

/// IVec so that we can use it easily in calculations
constexpr glm::ivec3 chunkDimensions {8,8,8};
constexpr unsigned chunkCount = chunkDimensions.x * chunkDimensions.y * chunkDimensions.z;
/// TODO: Instead of using default alloc, create a ChunkAlloc
//template <typename T>
//using ChunkSlice = std::vector<T>;
using ChunkContainer = std::vector<BlockType>;
    //ChunkSlice<ChunkSlice<ChunkSlice<BlockType,chunkDimensions.z>,chunkDimensions.y>, chunkDimensions.x>;
    //ChunkSlice<ChunkSlice<ChunkSlice<BlockType>>>;

// Send over the net
struct ChunkDataComponent
{
    ChunkDataComponent()
        : pos(0)
        , chunkIter(-1) 
    {
        //for (int i = 0; i < 6; i++) neighbor.v[i] = 0;
        //memset(neighbor.v, 0, sizeof(neighbor));
    }
    
    glm::ivec3 pos;
    ChunkContainer chunk;
    int chunkIter;
    bool isAir;

    bool operator==(const ChunkDataComponent& rhs) const { return true; }
};

template <typename S>
void serialize(S& s, ChunkDataComponent& o)
{
    serialize(s,o.pos);
    s.container1b(o.chunk, chunkCount);
    s.value4b(o.chunkIter);
    s.value1b(o.isAir);
}

struct ChunkSpawnCooldownComponent
{
    float time = 0.f;
};
inline bool operator==(const ChunkSpawnCooldownComponent& lhs, const ChunkSpawnCooldownComponent& rhs) { return true; }
template <typename S> void serialize(S& s, ChunkSpawnCooldownComponent& o) { s.value4b(o.time); }

struct ChunkSpawnComponent
{
    glm::ivec3 pos;
};
inline bool operator==(const ChunkSpawnComponent& lhs, const ChunkSpawnComponent& rhs) { return true; }
template <typename S> void serialize(S& s, ChunkSpawnComponent& o) { serialize(s, o.pos); }

struct ChunkMeshComponent
{
    ap::VertexArrayRef vao;
    int chunkIter = -1;
};
inline bool operator==(const ChunkMeshComponent& lhs, const ChunkMeshComponent& rhs) { return true; }
template <typename S> void serialize(S& s, ChunkMeshComponent& o) {}

struct ChunkModifiedComponent
{
    // When the local chunk is surrounded we want to force a rebuild
    bool force = false;
};
inline bool operator==(const ChunkModifiedComponent& lhs, const ChunkModifiedComponent& rhs) { return true; }
template <typename S> void serialize(S& s, ChunkModifiedComponent& o) {}

struct InputComponent
{
    bool mine = false;
    bool place = false;
    glm::ivec3 blockPos{ 0 };
};
inline bool operator==(const InputComponent& lhs, const InputComponent& rhs) { return true; }
template <typename S> void serialize(S& s, InputComponent& o) 
{
    serialize(s, o.blockPos);
    s.value1b(o.mine);
    s.value1b(o.place);
}

inline void RegisterChunkComponents()
{
    ap::Registry::RegisterComponent<ChunkDataComponent>();
    ap::Registry::RegisterComponent<ChunkMeshComponent>(
        //[](ap::Scene& scene, ap::Entity e) { AP_TRACE("Create mesh CB"); },
        //[](ap::Scene& scene, ap::Entity e) { AP_TRACE("Remove mesh CB"); }
    );
    ap::Registry::RegisterComponent<ChunkModifiedComponent>();
    ap::Registry::RegisterComponent<ChunkSpawnCooldownComponent>();
    ap::Registry::RegisterComponent<ChunkSpawnComponent>();
    ap::Registry::RegisterComponent<InputComponent>();
}

inline BlockType& GetBlock(ChunkContainer& chunk, unsigned x, unsigned y, unsigned z)
{
    return chunk.at((x * chunkDimensions.x * chunkDimensions.y) + (y * chunkDimensions.y) + z);
}

inline const BlockType& GetBlock(const ChunkContainer& chunk, unsigned x, unsigned y, unsigned z)
{
    return chunk.at((x * chunkDimensions.x * chunkDimensions.y) + (y * chunkDimensions.y) + z);
}

inline glm::ivec3 WorldToChunkCoordinates(const glm::ivec3& worldPos)
{
    auto mod = worldPos % glm::ivec3(chunkDimensions);
    // When we pass -2 we get back -2.
    // The problem here is that -2 is part or chunk -8
    // chunk pos = -2 - -2 is 0. It should be -8

     //To fix this, I add additional chunk offset to negative chunk coordinates
    if (mod.x < 0) mod.x = chunkDimensions.x + mod.x;
    if (mod.y < 0) mod.y = chunkDimensions.y + mod.y;
    if (mod.z < 0) mod.z = chunkDimensions.z + mod.z;

    return worldPos - mod;
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