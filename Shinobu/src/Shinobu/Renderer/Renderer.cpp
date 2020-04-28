#include "Shinobu/Renderer/Renderer.h"
#include "Shinobu/Renderer/RenderCommand.h"

namespace sh
{
    Renderer::SceneData Renderer::m_scene;

    void Renderer::Init()
    {
        RenderCommand::Init();
    }

    void Renderer::Shutdown()
    {    
    }
        
    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0,0,width,height);
    }

    void Renderer::BeginScene(const glm::mat4& viewProjection)
    {
        m_scene.viewProjectionMatrix = viewProjection;
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
    {
        shader->Bind();

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray,vertexArray->GetIndexBuffer()->GetCount());
    }
}