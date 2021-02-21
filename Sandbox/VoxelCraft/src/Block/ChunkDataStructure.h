#pragma once
#include "Component/ChunkComponent.h"

#include <Aphelion/Renderer/VertexArray.h>
#include <Aphelion/Renderer/Shader.h>
#include <Aphelion/Renderer/Texture.h>
#include <Aphelion/Renderer/PerspectiveCamera.h>
#include <Aphelion/Physics/RigidBody.h>

#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <queue>

struct ChunkData
{
    glm::ivec3 pos{ 0 };
    ChunkContainer data;

    // Make sure that we don't rebuild if we get the same chunk from server
    int chunkIter{ -1 };

    ap::VertexArrayRef vao{ nullptr };
    ap::RigidBody rb;
};

class ChunkDataStructure
{
public:
    ChunkDataStructure(ap::PhysicsScene& scene);

    void AddChunk(const ChunkDataComponent& chunk);
    void Update(float dt);
    void SetPos(const glm::vec3& playerPos);

    void Render(const ap::PerspectiveCamera& cam);

    unsigned ChunkRequestCount() const { return m_chunksToRequest.size(); }

private:
    enum Dir
    {
        Top,
        Left,
        Front,
        Bottom,
        Right,
        Back
    };

    std::array<ChunkData*,6> GetNeighbors(const glm::ivec3& chunkWorldPos);

    unsigned m_radius = 4;
    glm::ivec3 m_playerChunkPos{ 0 };

    std::unordered_map<glm::ivec3, ChunkData> m_chunks;
    std::deque<glm::ivec3> m_chunksToRequest;

    ap::ShaderRef m_shader;
    ap::TextureRef m_texture;
    ap::PhysicsScene& m_scene;
};