#include "Aphelion/ECS/Component.h"
#include "Aphelion/ECS/Registry.h"
#include "Aphelion/ECS/Scene.h"

namespace ap
{
    void APHELION_API RegisterECSComponents()
    {
        Registry::RegisterComponent<TransformComponent>();
        Registry::RegisterComponent<TagComponent>();
        Registry::RegisterComponent<GUIDComponent>();
        Registry::RegisterComponent<RigidBodyComponent>(
            [&](Scene& scene, Entity e)
            {
                scene.HandleComponentCreate(e, entt::type_info<RigidBodyComponent>::id());
            },
            [&](Scene& scene, Entity e)
            {
                scene.HandleComponentRemove(e, entt::type_info<RigidBodyComponent>::id());
            }
            );
    }
   
    void RigidBodyComponent::CreateStatic(const glm::mat4& transform)
    {
        AP_CORE_ASSERT(m_scene, "The scene is a nullptr");
        OnRemove();
        m_rb = RigidBody::CreateStatic(transform);
        // TODO: See if there is a way to do this without allocating.
        m_rb.SetUserData(new int(m_guid));
        m_scene->AddActor(m_rb);
    }
    
    void RigidBodyComponent::CreateDynamic(float density, const glm::mat4& transform)
    {
        AP_CORE_ASSERT(m_scene, "The scene is a nullptr");
        OnRemove();
        m_rb = RigidBody::CreateDynamic(density, transform);
        m_rb.SetUserData(new int(m_guid));
        m_scene->AddActor(m_rb);
    }
}