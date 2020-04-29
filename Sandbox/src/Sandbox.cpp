#include <Shinobu/Common.h>

#include <glm/gtc/matrix_transform.hpp>

// With this, build no longer works as shared library
//#include <imgui.h>
//#include <Shinobu/ImGui/ImGuiBuild.cpp>

class ExampleLayer : public sh::Layer
{
public:
    ExampleLayer() : sh::Layer("Example Layer") {}

    std::shared_ptr<sh::Texture> tex;
    float degrees = 0.f;

    virtual void OnAttach() override 
    {
        tex = sh::Texture2D::Create("res/image.png");
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
        sh::Renderer2D::BeginScene(glm::ortho(-2.f,2.f,2.f,-2.f));
        // Draw a quad
        sh::Renderer2D::Submit(sh::Render2DData(glm::vec2(0.f), glm::vec2(1.f), glm::vec4(1.f)));
        // Draw a rotated quad
        sh::Renderer2D::Submit(sh::Render2DData(glm::vec2(1.f,0.1f), glm::vec2(1.f), glm::vec4(0.5f,1.f,1.f,1.f), glm::radians(degrees)));
        // Draw a textured quad
        sh::Renderer2D::Submit(sh::Render2DData(glm::vec2(1.f), glm::vec2(3.f),tex, glm::radians(180.f)));
        sh::Renderer2D::EndScene();

        degrees += 1.f;

        //sh::Renderer::BeginScene(glm::mat4(1.f));
        //sh::Renderer::Submit(shader, array, glm::mat4(1));
        //sh::Renderer::EndScene();
    }


    virtual void OnGuiRender() override final
    {
        //ImGui::Begin("Data");
        //ImGui::SliderFloat("rotation", &degrees, 0, 360.f);
        //ImGui::End();
    }
};

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    auto app = std::make_unique<sh::Application>();
    app->GetLayerStack().PushLayer(new ExampleLayer);

    return app;
}