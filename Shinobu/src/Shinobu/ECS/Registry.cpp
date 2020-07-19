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
        SH_CORE_ASSERT(m_compData.count(packet.id) == 1, "Component is not registered or is incorrect");
        m_compData.at(packet.id).unpack(*this, entity, packet);
    }

    void Registry::Clone(Registry& from)
    {
        Get().clear();
        // Iterate over each entity in from registry
        from.Get().each([&](const entt::entity e)
            {
                // If entity does not exist create it
                // NOTE: This will pretty much always throw an exception since ENTT does not expect you to check for an entity
                // that has never been created. Because of this, I just always recreate each entity (which should be the same
                // runtime cost but I'm not worried about that atm)
                //if (Get().valid(e)) SH_CORE_VERIFY(Get().create(e) == e, "Could not copy entity from registry");

                // NOTE: For some reason I can't really copy a entity with all their values (such as version and id etc) so I 
                // ignore these details since I don't think we'll need them

                auto newE = Get().create(e);
                SH_CORE_VERIFY(e == newE, "Could not copy entity from registry");
                //SH_CORE_VERIFY(entt::to_integral(Get().create(e)) == entt::to_integral(e), "Could not copy entity from registry");
                //entt::to_integral(Get().create(e)) == entt::to_integral(e);

                // Copy all components into here
                from.Get().visit(e, [&](const entt::id_type component)
                    {
                        //SH_CORE_TRACE(m_compData.at(component).name);
                        m_compData.at(component).stamp(from.Get(), e, Get(), newE);
                        //Get().emplace_or_replace<T>(dst, from.get<T>(src));
                    });
            });
    }
}