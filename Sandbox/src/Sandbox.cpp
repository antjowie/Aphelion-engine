#include <Shinobu/Common.h>
#include <Shinobu/Event/KeyEvent.h>

#include <GLFW/glfw3.h>

class ExampleLayer : public sh::Layer
{
public:
    ExampleLayer() : sh::Layer("Example Layer") {}

    virtual void OnAttach() override { SH_INFO("Attached {0}", GetName()); }
    
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
            if (e.GetKeyCode() == GLFW_KEY_ESCAPE)
                sh::Application::Get().Exit();
        });

        //d.Dispatch<sh::LayerUpdateEvent>(SH_BIND_EVENT_FN(ExampleLayer::OnUpdate));
        //d.Dispatch<sh::LayerGuiRenderEvent>(SH_BIND_EVENT_FN(ExampleLayer::OnImGuiRender));
    }

private:
    void OnUpdate(sh::LayerUpdateEvent& event) { SH_TRACE("OnUpdate called in {0}", GetName()); }
    void OnImGuiRender(sh::LayerGuiRenderEvent& event) { SH_TRACE("OnImGuiRender called in {0}", GetName()); }
};

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    auto app = std::make_unique<sh::Application>();
    app->GetLayerStack().PushLayer(new ExampleLayer);

    return app;
}