#include <Shinobu/Common.h>

class ExampleLayer : public sh::Layer
{
public:
    ExampleLayer() : sh::Layer("Example Layer") {}

    std::shared_ptr<sh::Shader> shader;
    std::shared_ptr<sh::Texture> tex;
    std::shared_ptr<sh::VertexArray> array;

    virtual void OnAttach() override 
    {
        shader = sh::Shader::Create("res/shaders/Texture2D.glsl");
        //shader->Bind();
        tex = sh::Texture2D::Create("res/image.png");
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

        auto buffer = sh::VertexBuffer::Create(vert,sizeof(vert));
        buffer->AddElement(sh::BufferElement(sh::ShaderDataType::Float3, "aPos"));
        buffer->AddElement(sh::BufferElement(sh::ShaderDataType::Float2, "aTex", true));
        auto index = sh::IndexBuffer::Create(indices, 6);

        array = sh::VertexArray::Create();
        array->AddVertexBuffer(buffer);
        array->SetIndexBuffer(index);
    }
    
    virtual void OnDetach() override { SH_INFO("Detached {0}", GetName()); }

    virtual void OnEvent(sh::Event& event) override 
    {
        sh::EventDispatcher d(event); 
        d.Dispatch<sh::KeyTypedEvent>([](sh::KeyTypedEvent& e)
            {
                SH_TRACE("Typed key {0}", e.GetKeyCode());
            });

        d.Dispatch<sh::KeyPressedEvent>([&](sh::KeyPressedEvent& e)
        {
            if (e.GetKeyCode() == sh::KeyCode::Escape)
                sh::Application::Get().Exit();
        });
    }

    virtual void OnUpdate() override final
    {
        sh::Renderer::BeginScene(glm::mat4(1.f));
        tex->Bind();
        sh::Renderer::Submit(shader, array, glm::mat4(1));
        sh::Renderer::EndScene();
    }
    virtual void OnGuiRender() override final
    {
    }
};

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    auto app = std::make_unique<sh::Application>();
    app->GetLayerStack().PushLayer(new ExampleLayer);

    return app;
}