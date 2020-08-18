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
        for (auto& system : m_systems)
            system(*this);
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
}