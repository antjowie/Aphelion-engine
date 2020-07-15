#include "Shinobu/ECS/Scene.h"
#include "Scene.h"

namespace sh
{
    Registry& Scene::GetRegistry(unsigned offset)
    {
        SH_CORE_ASSERT(offset < maxRegistries, "Tried to roll world too far back. Exceeds max simulations that are stored");
        unsigned index = m_currentRegistry;

        // Make the index wrap around
        if (offset > index) 
        { 
            index = maxRegistries - (offset - index); 
        }
        else
        {
            index -= offset;
        }
        return m_registries[index];
    }
    unsigned Scene::GetSimulationCount() const
    {
        return m_simulationCount;
    }

    void Scene::Simulate(Timestep ts)
    {
        for (auto& system : m_systems)
            system(*this);

        //m_simulationCount++;
        //m_currentRegistry++;
        if (m_currentRegistry == maxRegistries)
            m_currentRegistry = 0;
    }
    void Scene::ClearSystems()
    {
        m_systems.clear();
    }
}