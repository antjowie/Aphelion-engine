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
    auto& reg = scene.GetRegistry();
    //auto view = reg.view<ChunkSpawnComponent>();

    reg.View<ChunkSpawnComponent>(
        [](ap::Entity e, ChunkSpawnComponent& spawnComp)
    {
        // TODO: These should not be stored as entities
        // Client will keep requesting entities
        ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(ap::Serialize(spawnComp, 0)));
    });

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
    auto& reg = scene.GetRegistry();
    //auto view = reg.view<ChunkSpawnComponent, SenderComponent>();

    reg.View<ChunkSpawnComponent, SenderComponent>(
        [&](ap::Entity e, ChunkSpawnComponent& spawnRequest, SenderComponent& sender)
    {   
        // Check if this chunk is already loaded
        ChunkDataComponent* targetChunkData = nullptr;
        ap::Entity targetChunkEntity;
        //auto chunkView = reg.view<ChunkDataComponent>();

        reg.View<ChunkDataComponent>(
            [&](ap::Entity chunk, ChunkDataComponent& chunkData)
        {
            if (chunkData.pos == spawnRequest.pos)
            {
                targetChunkData = &chunkData;
                targetChunkEntity = chunk;
                return;
            }
        });

        // If we can't find the chunk in our system, generate it
        if (!targetChunkData)
        {
            targetChunkEntity = scene.GetRegistry().Create();
            auto& chunkData = targetChunkEntity.AddComponent<ChunkDataComponent>();

            chunkData.chunk.resize(chunkCount);
            chunkData.pos = spawnRequest.pos;
            GenerateChunk(chunkData);

            targetChunkData = &chunkData;
        }

        ap::Application::Get().OnEvent(ap::ServerSendPacketEvent(ap::Serialize(
            *targetChunkData, targetChunkEntity.GetComponent<ap::GUIDComponent>().guid),sender.peer));
        ap::Application::Get().OnEvent(ap::ServerSendPacketEvent(ap::Serialize(
            ChunkModifiedComponent(), targetChunkEntity.GetComponent<ap::GUIDComponent>().guid), sender.peer));

        reg.Destroy(e);
    });
}

/**
 * A client side system to create chunks and their data
 */
inline void ChunkMeshBuilderSystem(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();
    
    reg.View<ChunkDataComponent, ChunkModifiedComponent>(
        [&](ap::Entity e, ChunkDataComponent& chunk, ChunkModifiedComponent& modified)
        {
            AP_TRACE("Building chunk id:{}", chunk.chunk.size(),e.GetComponent<ap::GUIDComponent>());
            // Generate the chunk vao
            if(!e.HasComponent<ChunkMeshComponent>()) e.AddComponent<ChunkMeshComponent>();
            auto& mesh = e.GetComponent<ChunkMeshComponent>();
            GenerateChunkMesh(chunk, mesh.vao);

            // We only do one chunk per frame for now
            e.RemoveComponent<ChunkModifiedComponent>();

            // TODO: This is a temp fix but once we build the chunk we remove any 
            // spawn request component. We probably want to add receive callbacks
            reg.View<ChunkSpawnComponent>(
                [&](ap::Entity e, ChunkSpawnComponent& spawnComp)
            {
                if (spawnComp.pos == chunk.pos) 
                {
                    reg.Destroy(e);
                    return;
                }
            });

            return;
        });
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
        auto& reg = scene.GetRegistry();

        ap::Renderer::BeginScene(m_cam);
        reg.View<ChunkDataComponent, ChunkMeshComponent>(
            [&](ap::Entity e, ChunkDataComponent& chunk, ChunkMeshComponent& mesh)
            {
                ap::Renderer::Submit(m_shader,mesh.vao,glm::translate(glm::identity<glm::mat4>(),chunk.pos));
            },
            entt::exclude<ChunkModifiedComponent>);
        ap::Renderer::EndScene();
    }

private:
    std::reference_wrapper<ap::PerspectiveCamera> m_cam;
    ap::ShaderRef m_shader;
    ap::TextureRef m_texture;
};