#include "Aphelion/Renderer/RenderCommand.h"

namespace ap
{
    std::unique_ptr<RendererAPI> RenderCommand::m_rendererAPI = RendererAPI::Create();
}