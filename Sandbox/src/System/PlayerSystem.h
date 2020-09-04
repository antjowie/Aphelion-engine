#pragma once
#include "Component/Component.h"

#include <Aphelion/Core/Application.h>
#include <Aphelion/ECS/Scene.h>
#include <Aphelion/Renderer/Renderer2D.h>

inline void PlayerView(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();

    // Render little crosshair
    static ap::OrthographicCamera ortho(-16, 16, -9, 9);
    ap::Renderer2D::BeginScene(ortho);
    ap::Render2DData data;
    data.size = glm::vec2(0.5f);
    data.color = glm::vec4(0, 255, 0, 255);
    ap::Renderer2D::Submit(data);
    ap::Renderer2D::EndScene();

    // Do a raycast from the player
    reg.View<ap::Transform, Player, ap::GUIDComponent>(
        [&](ap::Entity e, ap::Transform& t, Player&, ap::GUIDComponent& guid)
        {
            const auto& orig = t.GetPosition();
            const auto& dir = t.GetForward();
            const auto& distance = 10.f;
            
            auto hits = scene.Raycast(orig, dir, distance);
            for (const auto& hit : hits)
            {
                AP_TRACE("HIT");
            }
        });
}