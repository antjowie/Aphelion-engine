#include "Component/ChunkComponent.h"
#include "Component/ServerComponent.h"
#include "Component/Component.h"

#include "Layer/MainMenu.h"

#include "Block/BlockType.h"

#include <Shinobu/Core/EntryPoint.h>

std::unique_ptr<sh::Application> sh::CreateApplication()
{
    RegisterServerComponents();
    RegisterChunkComponents();
    RegisterComponents();

    RegisterBlocks();

    auto app = std::make_unique<sh::Application>();
    app->GetLayerStack().PushLayer(new MainMenuLayer());

    return app;
}