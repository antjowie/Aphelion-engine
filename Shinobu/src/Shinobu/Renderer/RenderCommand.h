#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/RendererAPI.h"

/**
 * Rendercommands are commands that we can query
 * By doing that, we know the whole draw state before
 * we are going to render it. This allows us to do
 * a bunch of optimizations such as batching materials
 *
 * TODO: Commands are just forwarded to the api.
 * We should buffer them and optimize based on that
 */
namespace sh
{
	class SHINOBU_API RenderCommand
	{
	public:
		static void Init()
		{
			m_rendererAPI->Init();
		}

		static void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height)
		{
			m_rendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(float r, float g, float b, float a)
		{
			m_rendererAPI->SetClearColor(r,g,b,a);
		}

		static void Clear()
		{
			m_rendererAPI->Clear();
		}

		static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, unsigned count = 0)
		{
			m_rendererAPI->DrawIndexed(vertexArray, count);
		}
	private:
		static std::unique_ptr<RendererAPI> m_rendererAPI;
	};
}