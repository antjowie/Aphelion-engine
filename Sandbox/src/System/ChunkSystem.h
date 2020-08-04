#pragma once
#include "Block/BlockLibrary.h"
#include "Component/ChunkComponent.h"
#include "Component/Component.h"
#include "Primitives.h"

#include <Shinobu/ECS/Scene.h>
#include <Shinobu/Renderer/Renderer.h>
#include <Shinobu/Renderer/Texture.h>

void GenerateChunk(ChunkDataComponent& chunk);
void GenerateChunkMesh(const ChunkDataComponent& chunk, sh::VertexArrayRef& vao);

/**
 * A system that decides which chunks to generate (and to remove)
 */
inline void ChunkStrategySystem(sh::Scene& scene)
{
    // Get player pos
    auto& reg = scene.GetRegistry().Get();
    //auto& player = reg.view<sh::Transform,Player>().get<sh::Transform>();
    auto playerView = reg.view<sh::Transform,Player>();
    if(!playerView.empty())
    {
        auto& player = playerView.get<sh::Transform>(playerView.front());

        // Generate chunks around player
        // TODO: Write this, for now I just manually spawn some chunks
    }
}

/**
 * A server side system to create chunks and their data
 */
inline void ChunkGenerateSystem(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry().Get();
    auto view = reg.view<ChunkSpawnComponent>();
    for(auto entity : view)
    {
        auto& spawnRequest = view.get<ChunkSpawnComponent>(entity);


        auto& chunkData = reg.get_or_emplace<ChunkDataComponent>(entity);

        if (chunkData.chunk.empty())
        {
            chunkData.chunk.resize(chunkCount);
            chunkData.pos = spawnRequest.pos;
            GenerateChunk(chunkData);

            // TEMP: add modified component, this should be send
            reg.emplace<ChunkModifiedComponent>(entity);
        }
    }
}

/**
 * A client side system to create chunks and their data
 */
inline void ChunkMeshBuilderSystem(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry().Get();
    auto view = reg.view<ChunkDataComponent, ChunkModifiedComponent>();
    for (auto entity : view)
    {
        auto& chunk = view.get<ChunkDataComponent>(entity);

        // Generate the chunk vao
        auto& mesh = reg.get_or_emplace<ChunkMeshComponent>(entity);
        GenerateChunkMesh(chunk, mesh.vao);

        // We only do one chunk per frame for now
        reg.remove<ChunkModifiedComponent>(entity);
        break;
    }
}

class ChunkRenderSystem
{
public:
    ChunkRenderSystem(std::reference_wrapper<sh::PerspectiveCamera> camera)
        : m_cam(std::move(camera))
        , m_shader(sh::Shader::Create("res/shader/Voxel.glsl"))
        , m_texture(sh::ArrayTexture2D::Create(2,2,"res/texture.png"))
    {

    }

    void operator() (sh::Scene& scene)
    {
        auto& reg = scene.GetRegistry().Get();
        auto view = reg.view<ChunkDataComponent, ChunkMeshComponent>(entt::exclude<ChunkModifiedComponent>);

        sh::Renderer::BeginScene(m_cam);
        for(auto entity : view)
        {
            auto& [chunk,mesh] = 
                view.get<ChunkDataComponent, ChunkMeshComponent>(entity);

            sh::Renderer::Submit(m_shader,mesh.vao,glm::translate(glm::identity<glm::mat4>(),chunk.pos));
        }
        sh::Renderer::EndScene();
    }

private:
    std::reference_wrapper<sh::PerspectiveCamera> m_cam;
    sh::ShaderRef m_shader;
    sh::TextureRef m_texture;
};