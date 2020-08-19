#include "Aphelion/Core/EntryPoint.h"
#include "PhysicsDemoLayer.h"

extern std::unique_ptr<ap::Application> ap::CreateApplication()
{
    auto app = std::make_unique<ap::Application>();
    app->GetLayerStack().PushLayer(new PhysicsDemoLayer());

    return app;
}
