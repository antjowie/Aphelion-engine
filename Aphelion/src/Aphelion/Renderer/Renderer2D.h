#pragma once

#include "Aphelion/Core/Core.h"
#include "Aphelion/Renderer/Texture.h "
#include "Aphelion/Renderer/OrthographicCamera.h"

#include <glm/glm.hpp>

namespace ap
{
    struct Render2DData
    {
        Render2DData() = default;
        Render2DData(const glm::vec2 & pos, const glm::vec2 & size, const glm::vec4 & color)
            : Render2DData(pos, size, color, 0) {}
        Render2DData(const glm::vec2 & pos, const glm::vec2 & size, const glm::vec4 & color, float radians)
            : pos(pos), size(size), color(color), radians(radians) {}
        Render2DData(const glm::vec2 & pos, const glm::vec2 & size, const ap::TextureRef& texture, const glm::vec4& tint = glm::vec4(1))
            : Render2DData(pos, size, texture, 0, tint) {}
        Render2DData(const glm::vec2& pos, const glm::vec2& size, const ap::TextureRef& texture, float radians, const glm::vec4 & tint = glm::vec4(1))
            : pos(pos), size(size), texture(texture), radians(radians), color(tint) {}

        glm::vec2 pos = glm::vec2(0.f);
        glm::vec2 size = glm::vec2(1.f);
        glm::vec4 color = glm::vec4(1.f);
        float radians = 0.f;
        ap::TextureRef texture = nullptr;
    };

    /**
     * TODO: All 2D data is drawn immediately. It should be batched.
     */
    class APHELION_API Renderer2D
    {
    public:
        static void Init();
        static void Deinit();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();

        static void Submit(const Render2DData& rData);
    };
}