#pragma once
#include "Block/BlockLibrary.h"
#include "Component/ChunkComponent.h"
#include "Component/Component.h"
#include "Primitives.h"
#include "Component/ServerComponent.h"

#include <Shinobu/Core/Application.h>
#include <Shinobu/ECS/Scene.h>
#include <Shinobu/Renderer/Renderer.h>
#include <Shinobu/Renderer/Texture.h>
#include <Shinobu/Event/NetEvent.h>

void GenerateChunk(ChunkDataComponent& chunk);
void GenerateChunkMesh(const ChunkDataComponent& chunk, sh::VertexArrayRef& vao);

/**
 * A system that decides which chunks to generate (and to remove)
 */
inline void ChunkStrategySystem(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry().Get();
    auto view = reg.view<ChunkSpawnComponent>();

    for (auto entity : view)
    {
        auto& spawnComp = view.get(entity);

        // TODO: These should not be stored as entities
        // Client will keep requesting entities
        sh::Application::Get().OnEvent(sh::ClientSendPacketEvent(sh::Serialize(spawnComp, sh::NullEntity, true)));
    }

    //auto& player = reg.view<sh::Transform,Player>().get<sh::Transform>();
    //auto playerView = reg.view<sh::Transform,Player>();
    //if(!playerView.empty())
    //{
    //    auto& player = playerView.get<sh::Transform>(playerView.front());

    //    // Generate chunks around player
    //    // TODO: Write this, for now I just manually spawn some chunks
    //}
}

/**
 * A server side system to respond on chunk requests
 */
inline void ChunkRequestResponseSystem(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry().Get();
    auto view = reg.view<ChunkSpawnComponent, SenderComponent>();
    for(auto entity : view)
    {
        auto& [spawnRequest, sender] = view.get<ChunkSpawnComponent,SenderComponent>(entity);
        
        // Check if this chunk is already loaded
        auto chunkView = reg.view<ChunkDataComponent>();
        ChunkDataComponent* targetChunkData = nullptr;
        sh::Entity targetChunkEntity;
        for (auto chunkEntity : chunkView)
        {
            auto& chunkData = chunkView.get(chunkEntity);
            if (chunkData.pos == spawnRequest.pos)
            {
                targetChunkData = &chunkData;
                targetChunkEntity = chunkEntity;
                break;
            }
        }

        // If we can't find the chunk in our system, generate it
        if (!targetChunkData)
        {
            auto chunk = reg.create();
            auto& chunkData = reg.get_or_emplace<ChunkDataComponent>(chunk);

            chunkData.chunk.resize(chunkCount);
            chunkData.pos = spawnRequest.pos;
            GenerateChunk(chunkData);

            targetChunkData = &chunkData;
            targetChunkEntity = chunk;
        }

        sh::Application::Get().OnEvent(sh::ServerSendPacketEvent(sh::Serialize(*targetChunkData, targetChunkEntity),sender.peer));
        sh::Application::Get().OnEvent(sh::ServerSendPacketEvent(sh::Serialize(ChunkModifiedComponent(), targetChunkEntity), sender.peer));
        reg.destroy(entity);
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

        // TODO: This is a temp fix but once we build the chunk we remove any 
        // spawn request component. We probably want to add receive callbacks
        auto spawnView = reg.view<ChunkSpawnComponent>();
        for (auto spawnRequest : spawnView)
        {
            auto& spawnComp = spawnView.get(spawnRequest);
            if (spawnComp.pos == chunk.pos) 
            {
                reg.destroy(spawnRequest);
                break;
            }
        }
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