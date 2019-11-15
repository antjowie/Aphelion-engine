#include <Shinobu/Common.h>

class ExampleLayer : public sh::Layer
{
public:
    ExampleLayer() : sh::Layer("Example Layer") {}

    virtual void OnAttach() override { SH_INFO("Attached {0}", GetName()); }
    
    virtual void OnDetach() override { SH_INFO("Detached {0}", GetName()); }

    virtual void OnEvent(sh::Event& event) override 
    {
        sh::EventDispatcher update(event); 
        update.Dispatch<sh::LayerUpdateEvent>(SH_BIND_EVENT_FN(ExampleLayer::OnUpdate));
        sh::EventDispatcher gui(event); 
        gui.Dispatch<sh::LayerGuiRenderEvent>(SH_BIND_EVENT_FN(ExampleLayer::OnImGuiRender));

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