#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Renderer/Texture.h "
#include "Shinobu/Renderer/OrthographicCamera.h"

#include <glm/glm.hpp>

namespace sh
{
    struct Render2DData
    {
        Render2DData() = default;
        Render2DData(const glm::vec2 & pos, const glm::vec2 & size, const glm::vec4 & color)
            : Render2DData(pos, size, color, 0) {}
        Render2DData(const glm::vec2 & pos, const glm::vec2 & size, const glm::vec4 & color, float radians)
            : pos(pos), size(size), color(color), radians(radians) {}
        Render2DData(const glm::vec2 & pos, const glm::vec2 & size, const std::shared_ptr<Texture> & texture, const glm::vec4 & tint = glm::vec4(1))
            : Render2DData(pos, size, texture, 0, tint) {}
        Render2DData(const glm::vec2 & pos, const glm::vec2 & size, const std::shared_ptr<Texture> & texture, float radians, const glm::vec4 & tint = glm::vec4(1))
            : pos(pos), size(size), texture(texture), radians(radians), color(tint) {}

        glm::vec2 pos;
        glm::vec2 size;
        glm::vec4 color;
        float radians;
        std::shared_ptr<Texture> texture;
    };

    /**
     * TODO: All 2D data is drawn immediately. It should be batched.
     */
    class SHINOBU_API Renderer2D
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();

        static void Submit(const Render2DData& rData);
    };
}