#include "Aphelion/ECS/Scene.h"
#include "Aphelion/Core/Time.h"

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
            vec.emplace_back(GetRegistry(0).Get(*reinterpret_cast<unsigned*>(hit.rb.GetUserData())), hit);
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
