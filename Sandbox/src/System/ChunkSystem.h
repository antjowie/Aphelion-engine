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
#include <Aphelion/Core/Event/NetEvent.h>

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

/// A client system that makes sure that we don't request the same chunk every frame
/// Maybe we want to refactor the chunk system to not rely on the ECS or be one object
inline void ChunkRequestCooldownSystem(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();

    reg.View<ChunkSpawnCooldownComponent>(
        [](ap::Entity e, ChunkSpawnCooldownComponent& cooldown)
    {
        cooldown.time += ap::Time::dt;

        // TODO: Should make this based on framerate or rtt I think
        if(cooldown.time > 0.1f)
            e.RemoveComponent<ChunkSpawnCooldownComponent>();
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

        // Generate the chunk vao
        if (!e.HasComponent<ChunkMeshComponent>()) e.AddComponent<ChunkMeshComponent>();
        auto& mesh = e.GetComponent<ChunkMeshComponent>();

        if (chunk.chunkIter == mesh.chunkIter)
            return;

        AP_TRACE("Building chunk {}", e.GetComponent<ap::GUIDComponent>());
        GenerateChunkMesh(chunk, mesh.vao);

        // Generate rigid body
        if (!e.HasComponent<ap::RigidBodyComponent>()) e.AddComponent<ap::RigidBodyComponent>();
        auto& physics = e.GetComponent<ap::RigidBodyComponent>();

        // Calculate stride of vbo
        auto& vbo = mesh.vao->GetVertexBuffer(0);
        auto& elements = vbo->GetElements();
        unsigned stride = 0;
        for (const auto& elem : elements)
            stride += elem.size;

        auto material = ap::PhysicsMaterial(1.f, 1.f, 1.f);
        auto shape = ap::PhysicsShape(
            ap::PhysicsGeometry::CreateTriangleMesh(
                vbo->GetData(),
                mesh.vao->GetIndexBuffer()->GetData(),
                stride),
                material/*,
                glm::translate(glm::identity<glm::mat4>(), -chunk.pos)*/);
        physics.CreateStatic(glm::translate(glm::identity<glm::mat4>(), chunk.pos /*+ (glm::vec3)chunkDimensions / 2.f*/));
        physics.GetRigidBody().AddShape(shape);

        e.GetComponent<ap::TagComponent>().tag = "Chunk";
        // We only do one chunk per frame for now
        e.RemoveComponent<ChunkModifiedComponent>();
        //e.AddComponent<ChunkSpawnCooldownComponent>();

        // TODO: This is a temp fix but once we build the chunk we remove any 
        // spawn request component. We probably want to add receive callbacks
        reg.View<ChunkSpawnComponent>(
            [&](ap::Entity spawnEntity, ChunkSpawnComponent& spawnComp)
        {
            if (spawnComp.pos == chunk.pos) 
            {
                reg.Destroy(spawnEntity);
                return;
            }
        });

        return;
    }, ap::typeList<ChunkSpawnCooldownComponent>);
}

class ChunkRenderSystem
{
public:
    ChunkRenderSystem(std::reference_wrapper<ap::PerspectiveCamera> camera)
        : m_cam(std::move(camera))
        , m_shader(ap::Shader::Create("res/shader/Voxel.glsl"))
        , m_texture(ap::ArrayTexture2D::Create(2,2,"res/texture.png"))
    {
        static const auto lightDir = glm::normalize(glm::vec3(0.1f, -1.f, -0.4f));
        m_shader->SetVec3("aLightDir", glm::value_ptr(lightDir));
        m_shader->SetFloat("aAmbient", 0.5f);
    }

    void operator() (ap::Scene& scene)
    {
        auto& reg = scene.GetRegistry();

        ap::Renderer::BeginScene(m_cam);
        reg.View<ChunkDataComponent, ChunkMeshComponent>(
            [&](ap::Entity e, ChunkDataComponent& chunk, ChunkMeshComponent& mesh)
        {
            if(mesh.vao)
            ap::Renderer::Submit(m_shader,mesh.vao,glm::translate(glm::identity<glm::mat4>(),chunk.pos));
        }
        //,ap::typeList<ChunkModifiedComponent>
        );
        ap::Renderer::EndScene();
    }

private:
    std::reference_wrapper<ap::PerspectiveCamera> m_cam;
    ap::ShaderRef m_shader;
    ap::TextureRef m_texture;
};

/////////////////////////////////////////////////////////////
// Server side
/////////////////////////////////////////////////////////////
/**
 * A server side system to respond on chunk requests
 */
inline void ChunkRequestResponseSystem(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();

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

                // Generate block types
                chunkData.chunk.resize(chunkCount);
                chunkData.pos = spawnRequest.pos;
                chunkData.chunkIter = 0;
                GenerateChunk(chunkData);

                targetChunkData = &chunkData;
            }

            auto& guid = targetChunkEntity.GetComponent<ap::GUIDComponent>().guid;
            //AP_INFO("Server send chunk {}", guid);
            ap::Application::Get().OnEvent(ap::ServerSendPacketEvent(ap::Serialize(
                *targetChunkData, guid), sender.peer));
            ap::Application::Get().OnEvent(ap::ServerSendPacketEvent(ap::Serialize(
                ChunkModifiedComponent(), guid), sender.peer));

            reg.Destroy(e);
        });
}

inline void BlockMineResponseSystem(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();

    reg.View<BlockMineComponent>(
        [&](ap::Entity e, BlockMineComponent& minedBlock)
        {
            auto blockWorldPos = glm::ivec3(minedBlock.blockPos);
            auto chunkPos = blockWorldPos - blockWorldPos % glm::ivec3(chunkDimensions);
            // Get the correct chunk
            reg.View<ChunkDataComponent>(
                [&](ap::Entity chunk, ChunkDataComponent& chunkData)
                {
                    AP_CORE_TRACE("{} {} {} {} {} {}", (int)chunkData.pos.x, (int)chunkData.pos.y, (int)chunkData.pos.z, (int)
                        chunkPos.x, (int)chunkPos.y, (int)chunkPos.z);
                    if (chunkData.pos == glm::vec3(chunkPos))
                    {
                        AP_CORE_INFO("FOUND");
                        auto blockPos = blockWorldPos - chunkPos;
                        GetBlock(chunkData.chunk, blockPos.x, blockPos.y, blockPos.z) = BlockType::Air;
                        chunkData.chunkIter++;

                        // TODO: Only send this to interested parties
                        ap::Application::Get().OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(
                            chunkData, chunk.GetComponent<ap::GUIDComponent>().guid)));
                        ap::Application::Get().OnEvent(ap::ServerBroadcastPacketEvent(ap::Serialize(
                            ChunkModifiedComponent(), chunk.GetComponent<ap::GUIDComponent>().guid)));

                        return;
                    }
                });
            
            reg.Destroy(e);
        });
}