#include "Aphelion/Renderer/Renderer2D.h"

#include "Aphelion/Renderer/RenderCommand.h"
#include "Aphelion/Renderer/VertexArray.h"
#include "Aphelion/Renderer/VertexBuffer.h"

#include "Aphelion/Renderer/Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <memory>

namespace ap
{
    struct Renderer2DData
    {
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Texture> whiteTexture;

        std::shared_ptr<VertexArray> vertexArray;
    };

    static Renderer2DData data;

	void Renderer2D::Init()
    {
        // Set up shader
        data.shader = Shader::Create("res/shader/Texture2D.glsl");
        data.whiteTexture = Texture2D::Create(1, 1);

        uint32_t white = 0xffffffff;
        data.whiteTexture->SetData(&white, sizeof(white));

        // Set up buffers
        constexpr float vert[] =
        {
            -0.5f, -0.5f, 0.f, 0.f,
             0.5f, -0.5f, 1.f, 0.f,
             0.5f,  0.5f, 1.f, 1.f,
            -0.5f,  0.5f, 0.f, 1.f,
        };

        constexpr uint32_t indices[] =
        { 0,1,2, 0,2,3 };

        auto buffer = ap::VertexBuffer::Create(vert, sizeof(vert));
        buffer->AddElement(ap::BufferElement(ap::ShaderDataType::Float2, "aPos"));
        buffer->AddElement(ap::BufferElement(ap::ShaderDataType::Float2, "aTex", true));
        auto index = ap::IndexBuffer::Create(indices, 6);
        
        data.vertexArray = ap::VertexArray::Create();
        data.vertexArray->AddVertexBuffer(buffer);
        data.vertexArray->SetIndexBuffer(index);
    }

	void Renderer2D::Shutdown()
    {
    }

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        data.shader->Bind();
        data.shader->SetMat4("aVP", glm::value_ptr(camera.GetViewProjectionMatrix()));
        
        data.vertexArray->Bind();
    }

	void Renderer2D::EndScene()
    {
    }

	void Renderer2D::Submit(const Render2DData& rData)
    {
        if(rData.texture) rData.texture->Bind();
        else data.whiteTexture->Bind();

        // Upload shader data
        glm::mat4 transform =
            glm::translate(glm::mat4(1), glm::vec3(rData.pos, 0)) *
            glm::rotate(glm::mat4(1), rData.radians, glm::vec3(0, 0, 1)) *
            glm::scale(glm::mat4(1), glm::vec3(rData.size, 1));

        data.shader->SetMat4("aTransform", glm::value_ptr(transform));
        data.shader->SetVec4("aTint", glm::value_ptr(rData.color));

        RenderCommand::DrawIndexed(data.vertexArray);
    }
}