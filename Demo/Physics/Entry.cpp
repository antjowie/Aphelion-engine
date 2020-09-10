#include "Aphelion/Core/EntryPoint.h"
#include "PhysicsDemoLayer.h"
#include "PhysicsRaycastLayer.h"

extern std::unique_ptr<ap::Application> ap::CreateApplication()
{
    auto app = std::make_unique<ap::Application>();
    app->GetLayerStack().PushLayer(new PhysicsDemoLayer());
    //app->GetLayerStack().PushLayer(new PhysicsRaycastLayer());

    return app;
}
