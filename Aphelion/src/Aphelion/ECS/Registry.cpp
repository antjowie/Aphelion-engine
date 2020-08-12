#include "Aphelion/ECS/Registry.h"

namespace ap
{
    std::unordered_map<entt::id_type, Registry::CompData> Registry::m_compData;
    PRNG Registry::m_prng = PRNG(std::chrono::system_clock::now().time_since_epoch().count());

    Entity Registry::Create(const std::string& tag)
    {
        auto guid = m_prng.Get();
        if (m_idToHandle.count(guid) == 1)
        {
            AP_CORE_WARN("Duplicate GUID generated. Attemting again...");            
            return Create(tag);
        }
        else if (guid == 0)
        {
            AP_CORE_WARN("Null GUID generated. Attemting again...");
            return Create(tag);
        }

        // Create the entity 

        auto handle = m_reg.create();
        Entity entity = { handle, m_reg };

        entity.AddComponent<TransformComponent>();
        entity.AddComponent<TagComponent>(tag);
        entity.AddComponent<GUIDComponent>(guid);
        
        m_idToHandle[guid] = handle;

        if (m_onCreate) m_onCreate(entity);
        return entity;
    }

    Entity Registry::Create(unsigned guid)
    {
        if (m_idToHandle.count(guid) == 1)
        {
            // In this case, a GUID has been received by a remote computer
            // What should we do in this case???
            // For now we crash
            AP_CORE_CRITICAL("Network duplicate GUID generated. Attemting again...");
        }

        auto handle = m_reg.create();
        Entity entity = { handle, m_reg };

        entity.AddComponent<TransformComponent>();
        entity.AddComponent<TagComponent>("network entity");
        entity.AddComponent<GUIDComponent>(guid);

        m_idToHandle[guid] = handle;

        if (m_onCreate) m_onCreate(entity);
        return entity;
    }

    void Registry::Destroy(Entity entity)
    {
        if (m_onDestroy) m_onDestroy(entity);

        auto handle = m_idToHandle.at(entity.GetComponent<GUIDComponent>().guid);
        m_idToHandle.erase(entity.GetComponent<GUIDComponent>().guid);

        m_reg.destroy(handle);
    }

    void Registry::HandlePacket(unsigned guid, Packet& packet)
    {
        //m_compData[compID].unpack(m_reg, entity, packet);
        AP_CORE_ASSERT(m_compData.count(packet.id) == 1, "Component is not registered or is incorrect");
        m_compData.at(packet.id).unpack(*this, m_idToHandle.at(guid), packet);
    }

    bool Registry::HandleAndReconcilePacket(unsigned id, Packet& packet)
    {
        AP_CORE_ASSERT(m_compData.count(packet.id) == 1, "Component is not registered or is incorrect");
        return m_compData.at(packet.id).unpackAndReconcile(*this, m_idToHandle.at(id), packet);
    }

    void Registry::Clone(Registry& from)
    {
        m_reg.clear();
        // Copy current GUID register (we may want to move this to scene)
        m_idToHandle = from.m_idToHandle;

        // Iterate over each entity in from registry
        from.m_reg.each([&](const entt::entity e)
            {
                // If entity does not exist create it
                // NOTE: This will pretty much always throw an exception since ENTT does not expect you to check for an entity
                // that has never been created. Because of this, I just always recreate each entity (which should be the same
                // runtime cost but I'm not worried about that atm)
                //if (Get().valid(e)) AP_CORE_VERIFY(Get().create(e) == e, "Could not copy entity from registry");

                // NOTE: For some reason I can't really copy a entity with all their values (such as version and id etc) so I 
                // ignore these details since I don't think we'll need them

                auto newE = m_reg.create(e);
                AP_CORE_VERIFY(e == newE, "Could not copy entity from registry");
                //AP_CORE_VERIFY(entt::to_integral(Get().create(e)) == entt::to_integral(e), "Could not copy entity from registry");
                //entt::to_integral(Get().create(e)) == entt::to_integral(e);

                // Copy all components into here
                from.m_reg.visit(e, [&](const entt::id_type component)
                    {
                        //AP_CORE_TRACE(m_compData.at(component).name);
                        AP_CORE_ASSERT(m_compData.count(component) == 1, "Component is not registered")
                        m_compData.at(component).stamp(from.m_reg, e, m_reg, newE);
                        //Get().emplace_or_replace<T>(dst, from.get<T>(src));
                    });
            });
    }
}