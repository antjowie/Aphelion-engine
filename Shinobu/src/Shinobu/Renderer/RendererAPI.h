#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/VertexArray.h"

#include <memory>

namespace sh
{
    /**
     * RendererAPI is the api wrapper of Shinobu. 
     * This is where our renderer classes will call to.
     * It will wrap render api functions such as clearing, viewport,
     * all the state that the renderers that we implement should need.
     */
    class SHINOBU_API RendererAPI
    {
    public:
        enum class API
        {
            None,
            OpenGL
        };

    public:
        virtual void Init() = 0;
        virtual void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height) = 0;
        virtual void SetClearColor(float r, float g, float b, float a) = 0;

        virtual void Clear() = 0;

        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

        static API GetAPI() { return m_api; }
        static std::unique_ptr<RendererAPI> Create();

    private:
        static API m_api;
    };
}