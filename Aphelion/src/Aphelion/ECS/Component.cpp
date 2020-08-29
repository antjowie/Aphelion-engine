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
        Registry::RegisterComponent<PhysicsComponent>(
            [&](Scene& scene, Entity e)
            {
                scene.HandleComponentCreate(e, entt::type_info<PhysicsComponent>::id());
            },
            [&](Scene& scene, Entity e)
            {
                scene.HandleComponentRemove(e, entt::type_info<PhysicsComponent>::id());
            }
            );
    }
   

    void PhysicsComponent::CreateStatic(PhysicsShape& shape, const glm::mat4& transform)
    {
        OnRemove();
        m_rb = RigidBody::CreateStatic(shape, transform);
        // TODO: See if there is a way to do this without allocating.
        m_rb.SetUserData(new int(m_guid));
        m_scene->AddActor(m_rb);
    }
    
    void PhysicsComponent::CreateDynamic(PhysicsShape& shape, float density, const glm::mat4& transform)
    {
        OnRemove();
        m_rb = RigidBody::CreateDynamic(shape, density, transform);
        m_rb.SetUserData(new int(m_guid));
        m_scene->AddActor(m_rb);
    }
}