#include "Aphelion/ECS/Scene.h"
#include "Aphelion/Core/Time.h"

#include <glm/gtx/matrix_decompose.hpp>

namespace ap
{
    /**
     * Makes sure that index is in range of the simulation array
     */
    unsigned GetSimulationIndex(unsigned currentSim, unsigned rollback)
    {
        //AP_CORE_ASSERT(rollback < Scene::maxSimulations, "Tried to roll world too far back. Exceeds max simulations that are stored");
        int targetIndex = currentSim - rollback;
        targetIndex %= Scene::maxSimulations;

        if (targetIndex < 0) targetIndex = Scene::maxSimulations - targetIndex;
        return targetIndex;
    }

    Scene::Scene()
        : m_physicsScene(PhysicsSceneDesc{})
    {
        for (size_t i = 0; i < maxSimulations; i++)
        {
            m_registries[i].SetScene(*this);
        }
    }

    Registry& Scene::GetRegistry(unsigned rollback)
    {
        return m_registries[GetSimulationIndex(m_currentSimulation,rollback)];
    }

    void Scene::Simulate(Timestep ts)
    {
        // 1. Copy current simulation values into next simulation
        // 2. Run the simulation
        // TODO: m_simulationCount can overflow
        auto nextSim = GetSimulationIndex(m_currentSimulation, -1);
        m_registries[nextSim].Clone(m_registries[m_currentSimulation]);

        m_simulationCount++;
        m_currentSimulation++;
        if (m_currentSimulation == maxSimulations)
            m_currentSimulation = 0;

        // Execute simulation
        m_physicsScene.Simulate(ts);

        // Update all transforms with physics transforms
        //GetRegistry(0).View<ap::TransformComponent, ap::RigidBodyComponent>(
        //    [](ap::Entity& e, ap::TransformComponent& t, ap::RigidBodyComponent& r)
        //    {
        //        if (!r.GetRigidBody()) return;

        //        auto rbT = r.GetRigidBody().GetWorldTransform();

        //        glm::vec3 scale;
        //        glm::quat rotation;
        //        glm::vec3 translation;
        //        glm::vec3 skew;
        //        glm::vec4 perspective;
        //        glm::decompose(rbT, scale, rotation, translation, skew, perspective);

        //        t.t.SetPosition(translation);
        //        //t.t.SetRotation(rotation);
        //    });

        for (auto& system : m_systems)
            system(*this);
    }

    std::vector<std::pair<Entity, PhysicsRaycastHit>> Scene::Raycast(const glm::vec3& origin, const glm::vec3& dir, float distance)
    {
        auto hit = m_physicsScene.Raycast(origin, dir, distance);
        
        // TODO: This always returns one hit
        std::vector<std::pair<Entity,PhysicsRaycastHit>> vec;
        if (hit)
        {
            void* userData = hit.rb.GetUserData();
            Entity e;
            if (userData) e = GetRegistry(0).Get(*reinterpret_cast<unsigned*>(userData));

            vec.emplace_back(e, hit);
        }

        return vec;
    }
    
    void Scene::ClearSystems()
    {
        m_systems.clear();
    }

    void Scene::SetOnEntityDestroyCb(Registry::EntityCb cb)
    {
        for(size_t i = 0; i < maxSimulations; i++)
            m_registries[i].SetOnEntityCreateCb(cb);
    }

    void Scene::SetOnEntityCreateCb(Registry::EntityCb cb)
    {
        for(size_t i = 0; i < maxSimulations; i++)
            m_registries[i].SetOnEntityDestroyCb(cb);
    }
    
    void Scene::HandleComponentCreate(Entity e, unsigned compID)
    {
        switch (compID)
        {
            case entt::type_info<RigidBodyComponent>::id():
            {
                e.GetComponent<RigidBodyComponent>().OnCreate(m_physicsScene, e.GetComponent<ap::GUIDComponent>());
            }
            break;
            
            default:
            AP_CORE_ASSERT(false, "HandleComponentCreate is called but is not handled in the Scene Handle function");
        }
    }
    
    void Scene::HandleComponentRemove(Entity e, unsigned compID)
    {
        switch(compID)
        {
            case entt::type_info<RigidBodyComponent>::id():
            {
                e.GetComponent<RigidBodyComponent>().OnRemove();
            }
            break;
            
            default:
            AP_CORE_ASSERT(false, "HandleComponentCreate is called but is not handled in the Scene Handle function");
        }
    }
}
