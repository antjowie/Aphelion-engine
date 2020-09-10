#include "Aphelion/Renderer/Renderer.h"
#include "Aphelion/Renderer/Renderer2D.h"
#include "Aphelion/Renderer/RenderCommand.h"

#include <glm/gtc/type_ptr.hpp>

namespace ap
{
    struct RendererData
    {
        //PerspectiveCamera camera;
        glm::mat4 view;
        glm::mat4 projection;
    };

    static RendererData data;

    void Renderer::Init()
    {
        RenderCommand::Init();
        Renderer2D::Init();
    }

    void Renderer::Deinit()
    {    
        Renderer2D::Deinit();
    }
        
    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0,0,width,height);
    }

    void Renderer::BeginScene(const PerspectiveCamera& camera)
    {
        data.view = camera.GetViewMatrix();
        data.projection = camera.GetProjectionMatrix();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const ShaderRef& shader, const VertexArrayRef& vertexArray, const glm::mat4& transform)
    {
        //shader->Bind();

        shader->SetMat4("aTransform", glm::value_ptr(transform));
        shader->SetMat4("aVP", glm::value_ptr(data.projection * data.view));
        
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray,vertexArray->GetIndexBuffer()->GetCount());
    }
}