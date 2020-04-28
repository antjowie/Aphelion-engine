#include "Shinobu/Renderer/RenderCommand.h"

namespace sh
{
    std::unique_ptr<RendererAPI> RenderCommand::m_rendererAPI = RendererAPI::Create();
}