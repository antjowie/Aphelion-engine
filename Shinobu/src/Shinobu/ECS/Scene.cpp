#include "Shinobu/ECS/Scene.h"
#include "Scene.h"

namespace sh
{
    /**
     * Makes sure that index is in range of the simulation array
     */
    unsigned GetSimulationIndex(unsigned currentSim, unsigned rollback)
    {
        //SH_CORE_ASSERT(rollback < Scene::maxSimulations, "Tried to roll world too far back. Exceeds max simulations that are stored");
        unsigned targetIndex = currentSim - rollback;
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
        // Execute simulation
        for (auto& system : m_systems)
            system(*this);

        //SH_CORE_TRACE("Simulation {}", m_currentSimulation);

        // --- Move on to next simulation ---
        // 1. Copy current simulation values into next simulation
        // 2. Increment simulation count
        // TODO: m_simulationCount can overflow
        auto nextSim = GetSimulationIndex(m_currentSimulation, -1);
        m_registries[nextSim].Clone(m_registries[m_currentSimulation]);

        m_simulationCount++;
        m_currentSimulation++;
        if (m_currentSimulation == maxSimulations)
            m_currentSimulation = 0;
    }
    
    void Scene::ClearSystems()
    {
        m_systems.clear();
    }
}