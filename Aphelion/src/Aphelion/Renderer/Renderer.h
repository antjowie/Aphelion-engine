#pragma once
#include "Aphelion/Core/Core.h"

#include "Aphelion/Renderer/RendererAPI.h"
#include "Aphelion/Renderer/Shader.h"
#include "Aphelion/Renderer/PerspectiveCamera.h"

#include <glm/glm.hpp>

#include <memory>

namespace ap
{
    /**
     * A renderer knows about the environment. It encapsulated the render flow.
     */
    class APHELION_API Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void OnWindowResize(uint32_t width, uint32_t height);

		// TODO: Upload scene data (lighting, camera, everything that is consistent for all objects in the scene)
		static void BeginScene(const PerspectiveCamera& camera);
		static void EndScene();

		static void Submit(const ShaderRef& shader, const VertexArrayRef& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};
}