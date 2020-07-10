#include "Shinobu/ECS/ECS.h"

namespace sh
{
    //std::unordered_map<entt::id_type, ECS::CloneFunc> ECS::m_cloneFns;
    std::vector<ECS::SystemFunc> ECS::m_systems;
    ECS::Registry ECS::m_reg;

    entt::registry& ECS::Registry::Get()
    {
        return m_reg;
    }
    
    void ECS::ClearSystems()
    {
        m_systems.clear();
    }

    void ECS::UpdateSystems()
    {
        for (auto& system : m_systems)
            system(m_reg);
    }
}