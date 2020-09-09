#pragma once
#include "Component/Component.h"

#include <Aphelion/Core/Application.h>
#include <Aphelion/ECS/Scene.h>
#include <Aphelion/Renderer/Renderer.h>
#include <Aphelion/Renderer/Primitive.h>
#include <Aphelion/Renderer/Renderer2D.h>

class PlayerView
{
public:
    PlayerView(ap::PerspectiveCamera& camera)
        : m_cam(camera)
        , m_shader(ap::Shader::Create("res/shader/Texture3DFlat.glsl"))
        , m_vao(ap::CreateCube())
    {
        m_shader->SetVec4("aColor", glm::value_ptr(glm::vec4(1.f,1.f,1.f,0.5f)));
    }

    void operator() (ap::Scene& scene)
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
        reg.View<ap::TransformComponent, Player, ap::GUIDComponent>(
            [&](ap::Entity e, ap::TransformComponent& t, Player&, ap::GUIDComponent& guid)
            {
                const auto& orig = t.t.GetPosition();
                const auto& dir = -t.t.GetForward(); // Camera faces opposite of view
                const auto& distance = 10.f;

                ap::Renderer::BeginScene(m_cam);
                auto hits = scene.Raycast(orig, dir, distance);
                for (const auto& hit : hits)
                {
                    //const auto& chunkPos = hit.first.GetComponent<ap::TransformComponent>().t.GetPosition();
                    auto chunk = hit.first;

                    // Calculate block index in array based on hit coordinates
                    glm::vec3 posInBlock = hit.second.pos - hit.second.normal * 0.5f + glm::vec3(0.5f);
                    //posInBlock -= chunkPos;
                    posInBlock = glm::ivec3(posInBlock);

                    glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), posInBlock);
                    ap::Renderer::Submit(m_shader, m_vao, glm::scale(translate, glm::vec3(1.1f)));
                    //AP_INFO("HIT");
                    InputComponent input;
                    static float cooldown = 0.f;

                    // Check if player places a block
                    if (ap::Input::IsButtonPressed(ap::ButtonCode::Right))
                    {
                        input.place = true;
                        input.blockPos = posInBlock + hit.second.normal;
                        ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(ap::Serialize(input, 0)));
                    }
                    // Check if player breaks a block
                    else if (cooldown == 0.f && ap::Input::IsButtonPressed(ap::ButtonCode::Left))
                    {
                        input.mine = true;
                        input.blockPos = posInBlock;
                        ap::Application::Get().OnEvent(ap::ClientSendPacketEvent(ap::Serialize(input, 0)));

                        cooldown = 0.2f;
                    }

                    cooldown -= ap::Time::dt;
                    if (cooldown < 0.f) cooldown = 0.f;
                }
                ap::Renderer::EndScene();
            });
    }

private:
    ap::PerspectiveCamera& m_cam;
    ap::ShaderRef m_shader;
    ap::VertexArrayRef m_vao;
};