#include "Shinobu/ECS/Registry.h"

namespace sh
{
    std::unordered_map<entt::id_type, Registry::CompData> Registry::m_compData;
    
    Entity Registry::Create()
    {
        return m_reg.create();
    }

    Entity Registry::Create(Entity hint)
    {
        auto id = m_reg.create(hint);
        // I've added this assert since there may be an issue when we predict new entities
        // locally but the server creates a new one. In this case, the ID's may not match. To solve this,
        // we either disallow the client from ever making entities themselves or we 
        // use a different ID system
        //
        // For example, a system that the Source engine uses is if the entity does not yet exist, 
        // create it. And only then. (I think)
        //
        // I HAVE A WAY BETTER SOLUTION!
        // Create a network ID component which stores the network ID of the entity.
        // This allows us to always refer to the same object
        SH_CORE_ASSERT(id == hint, "Could not recreate the hint ID");
        return id;
    }
    
    void Registry::HandlePacket(Entity entity, Packet& packet)
    {
        //m_compData[compID].unpack(m_reg, entity, packet);
        m_compData.at(packet.id).unpack(*this, entity, packet);
    }

    void Registry::ClearSystems()
    {
        m_systems.clear();
    }

    void Registry::UpdateSystems()
    {
        for (auto& system : m_systems)
            system(*this);
    }
}