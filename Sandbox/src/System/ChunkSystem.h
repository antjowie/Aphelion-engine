#pragma once
#include "Block/BlockLibrary.h"
#include "Component/ChunkComponent.h"
#include "Component/Component.h"
#include "Primitives.h"
#include "Component/ServerComponent.h"
#include "Block/ChunkDataStructure.h"

#include <Aphelion/Core/Application.h>
#include <Aphelion/ECS/Scene.h>
#include <Aphelion/Renderer/Renderer.h>
#include <Aphelion/Renderer/Texture.h>
#include <Aphelion/Core/Event/NetEvent.h>

void GenerateChunk(ChunkDataComponent& chunk);
//void GenerateChunkMesh(const ChunkDataComponent& chunk, ap::VertexArrayRef& vao);
/**
 * A system that decides which chunks to generate (and to remove)
 */
class ChunkHandlerSystem
{
public:
    ChunkHandlerSystem(ChunkDataStructure& chunks)
        : m_chunks(chunks) {}

    void operator() (ap::Scene& scene)
    {
        auto& reg = scene.GetRegistry();
        reg.View<ChunkDataComponent>(
            [&](ap::Entity e, ChunkDataComponent& data)
            {
                m_chunks.AddChunk(data);

                // TEMP: Spawn will be a command, should not be in ecs
                reg.View<ChunkSpawnComponent>(
                    [&](ap::Entity spawnEntity, ChunkSpawnComponent& spawnComp)
                    {
                        if (spawnComp.pos == data.pos)
                        {
                            reg.Destroy(spawnEntity);
                            return;
                        }
                    });

                reg.Destroy(e);
            });

        // TEMP: This will normally be done by the chunk data structure
        reg.View<ChunkSpawnComponent>(
            [](ap::Entity e, ChunkSpawnComponent& spawnComp)
            {
                // TODO: These should not be stored as entities
                // Client will keep requesting entities
                ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(ap::Serialize(spawnComp, 0)));
            });
    }

private:
    ChunkDataStructure& m_chunks;
};

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

            if (!modified.force && chunk.chunkIter == mesh.chunkIter)
            {
                //AP_TRACE("Old chunk new:{} old:{} {}", chunk.chunkIter, mesh.chunkIter, e.GetComponent<ap::GUIDComponent>());
                return;
            }

            mesh.chunkIter = chunk.chunkIter;
            e.GetComponent<ap::TagComponent>().tag = "Chunk";

            AP_TRACE("Building chunk new:{} old:{} {}", chunk.chunkIter, mesh.chunkIter, e.GetComponent<ap::GUIDComponent>());

            if (chunk.isAir)
                return;
        
            //GenerateChunkMesh(chunk, mesh.vao);

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
            physics.CreateStatic(glm::translate(glm::identity<glm::mat4>(), glm::vec3(chunk.pos) /*+ (glm::vec3)chunkDimensions / 2.f*/));
            physics.GetRigidBody().AddShape(shape);

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

            // We only do one chunk per frame for now
            e.RemoveComponent<ChunkModifiedComponent>();
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
            if(!chunk.isAir && mesh.vao)
            ap::Renderer::Submit(m_shader,mesh.vao,glm::translate(glm::identity<glm::mat4>(),glm::vec3(chunk.pos)));
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

inline void InputResponseSystem(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();

    reg.View<InputComponent>(
        [&](ap::Entity e, InputComponent& input)
        {
            auto blockWorldPos = input.blockPos;
            // NOTE: Negative mod returns a positive number. I'd expect a negative, so because of that. We want to subract in case of negative values
            auto mod = blockWorldPos % glm::ivec3(chunkDimensions);
            // For example block pos -13 mod 8 returns 5 but I want -5 so update the sign
            // -13 - -5 = -8 which is our chunk pos
            //mod *= (blockWorldPos / glm::abs(blockWorldPos)); // Make sure to retain the sign
            auto chunkWorldPos = blockWorldPos - mod;
            // Get the correct chunk
            reg.View<ChunkDataComponent>(
                [&](ap::Entity chunk, ChunkDataComponent& chunkData)
                {
                    //AP_CORE_TRACE("{} {} {} {} {} {}", (int)chunkData.pos.x, (int)chunkData.pos.y, (int)chunkData.pos.z, (int)
                    //    chunkPos.x, (int)chunkPos.y, (int)chunkPos.z);
                    if (chunkData.pos == chunkWorldPos)
                    {
                        //AP_CORE_INFO("FOUND");
                        auto blockChunkPos = blockWorldPos - chunkWorldPos;
                        
                        if(input.mine)
                            GetBlock(chunkData.chunk, blockChunkPos.x, blockChunkPos.y, blockChunkPos.z) = BlockType::Air;
                        else
                            GetBlock(chunkData.chunk, blockChunkPos.x, blockChunkPos.y, blockChunkPos.z) = BlockType::Stone;
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