#include "Shinobu/Core/Application.h"

#include "Shinobu/ImGui/ImGuiLayer.h"

#include "Shinobu/Renderer/RendererAPI.h"
#include "Shinobu/Renderer/RenderCommand.h"

// TEMP
#include "Shinobu/Renderer/Shader.h"
#include "Shinobu/Renderer/VertexArray.h"
#include "Shinobu/Renderer/VertexBuffer.h"
#include "Shinobu/Renderer/Texture.h"

namespace sh
{
    Application* Application::m_instance = nullptr;

    Application::Application()
        : m_imguiLayer(nullptr)
        , m_isRunning(true)
    {
        SH_CORE_ASSERT(!m_instance,"Can not create multiple Applications");
        m_instance = this;

        m_window = Window::Create();
        m_window->SetEventCallback(SH_BIND_EVENT_FN(Application::OnEvent));
        
        m_imguiLayer = new ImGuiLayer();
        m_layerStack.PushOverlay(m_imguiLayer);
    }

    void Application::Run()
    {
        RenderCommand::Init();
        RenderCommand::SetClearColor(0.5f, 0.f, 0.5f, 1.f);


        auto shader = Shader::Create("default",
R"(
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 tex;

void main()
{
    tex = aTex;
    gl_Position = vec4(aPos ,1.0f);
}
)",

R"(
#version 450 core

uniform sampler2D sampl;

in vec2 tex;
out vec4 color;

void main()
{
	color = texture(sampl, tex);
}
)");
        shader->Bind();
        auto tex = Texture2D::Create("image.png");
        tex->Bind();

        //glDebugMessageInsert(0, 0, 0, 0, 100, "OOF");

        constexpr float vert[] =
        {
            -0.5f, -0.5f, 0.f, 0.f, 0.f,
             0.5f, -0.5f, 0.f, 1.f, 0.f,
             0.5f,  0.5f, 0.f, 1.f, 1.f,
            -0.5f,  0.5f, 0.f, 0.f, 1.f,
        };

        constexpr uint32_t indices[] =
        { 0,1,2, 0,2,3 };

        auto buffer = VertexBuffer::Create(vert,sizeof(vert));
        buffer->AddElement(BufferElement(ShaderDataType::Float3, "aPos"));
        buffer->AddElement(BufferElement(ShaderDataType::Float2, "aTex", true));
        auto index = IndexBuffer::Create(indices, 6);

        auto array = VertexArray::Create();
        array->AddVertexBuffer(buffer);
        array->SetIndexBuffer(index);

        while (m_isRunning)
        {
            m_imguiLayer->Begin();
            
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(array);

            for (auto layer = m_layerStack.begin(); layer != m_layerStack.end(); layer++)
            {
                (*layer)->OnEvent(LayerUpdateEvent());
                (*layer)->OnEvent(LayerGuiRenderEvent());
            }

            m_imguiLayer->End();

            m_window->OnUpdate();
        }
    }

    void Application::OnEvent(Event& event)
    {
        sh::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(SH_BIND_EVENT_FN(Application::OnWindowClose));
       
        // TODO: Make layertype be of type standard container to support standard container opperations (rend in this case)
        for (auto layer = m_layerStack.end() - 1; /*layer != m_layerStack.begin() - 1*/; layer--)
        {
            if(event.m_handled) return;
            (*layer)->OnEvent(event);

            // Calling -- on begin() is incorrect behavior
            if (layer == m_layerStack.begin()) return;
        }
    }
    
    void Application::OnWindowClose(WindowCloseEvent& event)
    {
        m_isRunning = false;
    }
    

} // namespace sh
