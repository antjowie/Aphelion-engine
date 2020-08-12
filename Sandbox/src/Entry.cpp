#include "Component/ChunkComponent.h"
#include "Component/ServerComponent.h"
#include "Component/Component.h"

#include "Layer/MainMenu.h"

#include "Block/BlockType.h"

#include <Aphelion/Core/EntryPoint.h>

std::unique_ptr<ap::Application> ap::CreateApplication()
{
    /// TODO: Move component registry or use a macro
    ap::Registry::RegisterComponent<ap::TransformComponent>();
    ap::Registry::RegisterComponent<ap::TagComponent>();
    ap::Registry::RegisterComponent<ap::GUIDComponent>();

    RegisterServerComponents();
    RegisterChunkComponents();
    RegisterComponents();

    RegisterBlocks();

    auto app = std::make_unique<ap::Application>();
    app->GetLayerStack().PushLayer(new MainMenuLayer());

    return app;
}