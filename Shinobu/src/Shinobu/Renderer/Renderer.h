#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/RendererAPI.h"

#include "Shinobu/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <memory>

namespace sh
{
    /**
     * The API that the end user will use.
     * It wraps calls to the graphics API.
     */
    class SHINOBU_API Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const glm::mat4& viewProjection);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

        static SceneData m_scene;
	};
}