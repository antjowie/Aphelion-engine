#include "Shinobu/ECS/ECS.h"

namespace sh
{
    std::unordered_map<entt::id_type, ECS::CompData> ECS::m_compData;
    std::vector<ECS::SystemFunc> ECS::m_systems;
    ECS::Registry ECS::m_reg;

    ECS::Entity ECS::Registry::Create(Entity hint)
    {
        auto id = m_reg.create(hint);
        // I've added this assert since there may be an issue when we predict new entities
        // locally but the server creates a new one. In this case, the ID's may not match. To solve this,
        // we either disallow the client from ever making entities themselves or we 
        // use a different ID system
        //
        // For example, a system that the Source engine uses is if the entity does not yet exist, 
        // create it. And only then. (I think)
        SH_CORE_ASSERT(id == hint, "Could not recreate the hint ID");
        return id;
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

    const std::unordered_map<entt::id_type, ECS::CompData>& ECS::GetComponentData()
    {
        return m_compData;
    }
}