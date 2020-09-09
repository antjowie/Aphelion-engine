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

// Hash func for all vec3 types
namespace std
{   
    template<typename T> struct hash<glm::vec<3,T>>
    {
        std::size_t operator()(glm::vec<3, T> const& v) const noexcept
        {
            std::size_t h1 = /*std::hash<T>*/(v.x);
            std::size_t h2 = /*std::hash<T>*/(v.y);
            std::size_t h3 = /*std::hash<T>*/(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 10); // or use boost::hash_combine
        }
    };
}

/**
 * The strategy is responsible for setting up the neigbors of a chunk
 * All new chunks and removed chunks are forwarded to here
 * Then it adds chunk modified components when needed
 * and it makes chunk spawn requests
 * 
 * It does not generate mesh data
 * This is handled by the ChunkMeshGenerateSystem
 */
class ChunkStrategy
{
public:
    struct ChunkProxy
    {
        unsigned chunkGUID;

        union
        {
            ChunkProxy* v[6];
            struct
            {
                ChunkProxy* top;
                ChunkProxy* left;
                ChunkProxy* front;
                ChunkProxy* bottom;
                ChunkProxy* right;
                ChunkProxy* back;
            };
        }neighbor;
        unsigned neighborCount = 0;

        //bool operator== (const ChunkProxy& rhs) const { return chunkGUID == rhs.chunkGUID;}
    };

    ChunkStrategy()
    {
        m_chunks.clear();
        m_chunksNotFilled.clear();
    }

    static void AddChunk(ap::Entity& chunk);
    // This WILL remove the chunk entity
    static void RemoveChunk(ap::Entity& chunk);
    void operator() (ap::Scene& scene);

    /// Radius in chunk units
    static float m_radius;
    // This has to be set up in the client
    static ap::Scene* m_scene;

private:
    static std::unordered_map<glm::ivec3, ChunkProxy> m_chunks;
    static std::list<std::reference_wrapper<ChunkProxy>> m_chunksNotFilled;
};

inline bool operator== (const ChunkStrategy::ChunkProxy& lhs,const ChunkStrategy::ChunkProxy& rhs) { return lhs.chunkGUID == rhs.chunkGUID; }


/**
 * A system that decides which chunks to generate (and to remove)
 */
inline void ChunkStrategySystem(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();

    //auto view = reg.view<ChunkSpawnComponent>();


    // Sync chunk proxies with actual chunks
    // TODO: We should add a callback for when we receive certain components
    //reg.View<ChunkDataComponent>(
    //    [](ap::Entity e, ChunkDataComponent& data)
    //    {
    //        if (chunks.count(data.pos) == 0)
    //        {
    //            chunks[data.pos].chunkID = e.GetComponent<ap::GUIDComponent>();
    //        }
    //        //data.pos
    //    });

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

            //ChunkStrategy::AddChunk(e);
            mesh.chunkIter = chunk.chunkIter;
            e.GetComponent<ap::TagComponent>().tag = "Chunk";

            AP_TRACE("Building chunk new:{} old:{} {}", chunk.chunkIter, mesh.chunkIter, e.GetComponent<ap::GUIDComponent>());

            if (chunk.isAir)
                return;
        
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
            auto blockWorldPos = glm::ivec3(input.blockPos);
            auto chunkPos = blockWorldPos - blockWorldPos % glm::ivec3(chunkDimensions);
            // Get the correct chunk
            reg.View<ChunkDataComponent>(
                [&](ap::Entity chunk, ChunkDataComponent& chunkData)
                {
                    //AP_CORE_TRACE("{} {} {} {} {} {}", (int)chunkData.pos.x, (int)chunkData.pos.y, (int)chunkData.pos.z, (int)
                    //    chunkPos.x, (int)chunkPos.y, (int)chunkPos.z);
                    if (chunkData.pos == chunkPos)
                    {
                        //AP_CORE_INFO("FOUND");
                        auto blockPos = blockWorldPos - chunkPos;
                        
                        if(input.mine)
                            GetBlock(chunkData.chunk, blockPos.x, blockPos.y, blockPos.z) = BlockType::Air;
                        else
                            GetBlock(chunkData.chunk, blockPos.x, blockPos.y, blockPos.z) = BlockType::Stone;
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