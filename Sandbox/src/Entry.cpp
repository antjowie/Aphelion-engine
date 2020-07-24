#include "Component/Component.h"
#include "Layer/MainMenu.h"

#include <Shinobu/Core/EntryPoint.h>
#include <Shinobu/ECS/Registry.h>

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    sh::Registry::RegisterComponent<Player>();
    sh::Registry::RegisterComponent<Transform>();
    sh::Registry::RegisterComponent<Sprite>();
    sh::Registry::RegisterComponent<Health>();
    auto app = std::make_unique<sh::Application>();
    app->GetLayerStack().PushLayer(new MainMenuLayer());

    return app;
}