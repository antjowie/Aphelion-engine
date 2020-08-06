#pragma once
#include "Block/BlockLibrary.h"
#include "Component/ChunkComponent.h"
#include "Component/Component.h"
#include "Primitives.h"
#include "Component/ServerComponent.h"

#include <Aphelion/Core/Application.h>
#include <Aphelion/ECS/Scene.h>
#include <Aphelion/Renderer/Renderer.h>
#include <Aphelion/Renderer/Texture.h>
#include <Aphelion/Event/NetEvent.h>

void GenerateChunk(ChunkDataComponent& chunk);
void GenerateChunkMesh(const ChunkDataComponent& chunk, ap::VertexArrayRef& vao);

/**
 * A system that decides which chunks to generate (and to remove)
 */
inline void ChunkStrategySystem(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry().Get();
    auto view = reg.view<ChunkSpawnComponent>();

    for (auto entity : view)
    {
        auto& spawnComp = view.get(entity);

        // TODO: These should not be stored as entities
        // Client will keep requesting entities
        ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(ap::Serialize(spawnComp, ap::NullEntity, true)));
    }

    //auto& player = reg.view<ap::Transform,Player>().get<ap::Transform>();
    //auto playerView = reg.view<ap::Transform,Player>();
    //if(!playerView.empty())
    //{
    //    auto& player = playerView.get<ap::Transform>(playerView.front());

    //    // Generate chunks around player
    //    // TODO: Write this, for now I just manually spawn some chunks
    //}
}

/**
 * A server side system to respond on chunk requests
 */
inline void ChunkRequestResponseSystem(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry().Get();
    auto view = reg.view<ChunkSpawnComponent, SenderComponent>();
    for(auto entity : view)
    {
        auto& [spawnRequest, sender] = view.get<ChunkSpawnComponent,SenderComponent>(entity);
        
        // Check if this chunk is already loaded
        auto chunkView = reg.view<ChunkDataComponent>();
        ChunkDataComponent* targetChunkData = nullptr;
        ap::Entity targetChunkEntity;
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

        ap::Application::Get().OnEvent(ap::ServerSendPacketEvent(ap::Serialize(*targetChunkData, targetChunkEntity),sender.peer));
        ap::Application::Get().OnEvent(ap::ServerSendPacketEvent(ap::Serialize(ChunkModifiedComponent(), targetChunkEntity), sender.peer));
        reg.destroy(entity);
    }
}

/**
 * A client side system to create chunks and their data
 */
inline void ChunkMeshBuilderSystem(ap::Scene& scene)
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
    ChunkRenderSystem(std::reference_wrapper<ap::PerspectiveCamera> camera)
        : m_cam(std::move(camera))
        , m_shader(ap::Shader::Create("res/shader/Voxel.glsl"))
        , m_texture(ap::ArrayTexture2D::Create(2,2,"res/texture.png"))
    {

    }

    void operator() (ap::Scene& scene)
    {
        auto& reg = scene.GetRegistry().Get();
        auto view = reg.view<ChunkDataComponent, ChunkMeshComponent>(entt::exclude<ChunkModifiedComponent>);

        ap::Renderer::BeginScene(m_cam);
        for(auto entity : view)
        {
            auto& [chunk,mesh] = 
                view.get<ChunkDataComponent, ChunkMeshComponent>(entity);

            ap::Renderer::Submit(m_shader,mesh.vao,glm::translate(glm::identity<glm::mat4>(),chunk.pos));
        }
        ap::Renderer::EndScene();
    }

private:
    std::reference_wrapper<ap::PerspectiveCamera> m_cam;
    ap::ShaderRef m_shader;
    ap::TextureRef m_texture;
};