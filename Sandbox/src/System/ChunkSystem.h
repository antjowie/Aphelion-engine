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
 * A system to create chunks and their data and mesh
 */
inline void ChunkGenerateSystem(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry().Get();
    auto view = reg.view<ChunkDataComponent, ChunkMeshComponent,ChunkModifiedComponent>();
    for(auto entity : view)
    {
        auto& [chunk,mesh,flag] = 
            view.get<ChunkDataComponent, ChunkMeshComponent,ChunkModifiedComponent>(entity);

        // TODO: Note for next time. Chunks cause stack overflow.
        // TEMP: Generate the chunk block data
        GenerateChunk(chunk);

        // Generate the chunk vao
        GenerateChunkMesh(chunk,mesh.vao);

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