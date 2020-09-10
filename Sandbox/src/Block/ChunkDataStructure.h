#pragma once
#include "Component/ChunkComponent.h"

#include <Aphelion/Renderer/VertexArray.h>
#include <Aphelion/Renderer/Shader.h>
#include <Aphelion/Renderer/Texture.h>
#include <Aphelion/Renderer/PerspectiveCamera.h>
#include <Aphelion/Physics/RigidBody.h>

#include <glm/gtx/hash.hpp>
#include <unordered_map>

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
    void Update();

    void Render(const ap::PerspectiveCamera& cam);

private:
    std::unordered_map<glm::ivec3, ChunkData> m_chunks;

    ap::ShaderRef m_shader;
    ap::TextureRef m_texture;
    ap::PhysicsScene& m_scene;
};