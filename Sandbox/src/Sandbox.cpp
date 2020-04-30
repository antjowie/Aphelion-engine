#include <Shinobu/Common.h>

class ExampleLayer : public sh::Layer
{
public:
    ExampleLayer() 
        : sh::Layer("Example Layer") 
        , m_camera(16.f/9.f){}

    std::shared_ptr<sh::Texture> tex;
    float degrees = 0.f;
    float yPos = 0.f;
    sh::OrthographicCameraController m_camera;

    virtual void OnAttach() override 
    {
        // 1) Important: globals are not shared across DLL boundaries! If you use DLLs or any form of hot-reloading: you will need to call
        //    SetCurrentContext() (with the pointer you got from CreateContext) from each unique static/DLL boundary, and after each hot-reloading.
        //    In your debugger, add GImGui to your watch window and notice how its value changes depending on which location you are currently stepping into.
        ImGui::SetCurrentContext(sh::ImGuiLayer::GetContext());
        tex = sh::Texture2D::Create("res/image.png");
    }
    
    virtual void OnDetach() override { SH_INFO("Detached {0}", GetName()); }

    virtual void OnEvent(sh::Event& event) override 
    {
        m_camera.OnEvent(event);

        sh::EventDispatcher d(event); 
        d.Dispatch<sh::KeyPressedEvent>([&](sh::KeyPressedEvent& e)
        {
            if (e.GetKeyCode() == sh::KeyCode::Escape)
                sh::Application::Get().Exit();
            return false;
        });
    }


    virtual void OnUpdate(sh::Timestep ts) override final
    {
        m_camera.OnUpdate(ts);

        sh::Renderer2D::BeginScene(m_camera.GetCamera());
        // Draw a quad
        sh::Renderer2D::Submit(sh::Render2DData(glm::vec2(0.f), glm::vec2(1.f), glm::vec4(1.f)));
        // Draw a rotated quad
        sh::Renderer2D::Submit(sh::Render2DData(glm::vec2(1.f,0.1f), glm::vec2(1.f), glm::vec4(0.5f,1.f,1.f,1.f), glm::radians(degrees)));
        // Draw a textured quad
        sh::Renderer2D::Submit(sh::Render2DData(glm::vec2(0.f,yPos), glm::vec2(3.f),tex));
        sh::Renderer2D::EndScene();
    }


    virtual void OnGuiRender() override final
    {
        if (!ImGui::Begin("Renderer 2D"))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }
        ImGui::SliderFloat("rotation", &degrees, 0, 360.f);
        ImGui::SliderFloat("yPos", &yPos,-10.f,10.f);
        ImGui::End();
    }
};

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    auto app = std::make_unique<sh::Application>();
    app->GetLayerStack().PushLayer(new ExampleLayer);

    return app;
}