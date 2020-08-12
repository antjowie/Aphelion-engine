#pragma once
#include "Aphelion/Core/Core.h"

#include <entt/entt.hpp>

namespace ap
{
    /**
     * A handle into a container of components. Don't store entities since they get invalidated every frame
     * 
     * Entities are constructed every frame. They contain functions to modify the entity they
     * are pointing towards. 
     * Internally, they hold a registry. Since we switch registries every frame, entities are no longer
     * of any importance during the next frame. Doing so could even break reconcile behavior. 
     * 
     * If you want to reference other entities (for parenting purposes) use a ParentComponent which contains the 
     * entity ID
     */
    class APHELION_API Entity
    {
    public:
        Entity() {}
        Entity(const entt::entity handle, entt::registry& registry) 
            : m_handle(handle)
            , m_reg(&registry)
        {}

        template <typename Component, typename... Args>
        Component& AddComponent(Args&&... args)
        {
            AP_CORE_ASSERT(*this, "Entity is not valid");
            return m_reg->emplace<Component>(m_handle, std::forward<Args>(args)...);
        }

        template <typename Component>
        void RemoveComponent()
        {
            AP_CORE_ASSERT(*this, "Entity is not valid");
            AP_CORE_ASSERT(m_reg->has<Component>(m_handle), "Entity does not have that component");
            m_reg->remove<Component>(m_handle);
        }

        template <typename Component>
        Component& GetComponent()
        {
            AP_CORE_ASSERT(*this, "Entity is not valid");
            return m_reg->get<Component>(m_handle);
        }

        template <typename Component>
        const Component& GetComponent() const
        {
            AP_CORE_ASSERT(*this, "Entity is not valid");
            return m_reg->get<Component>(m_handle);
        }

        template <typename Component>
        bool HasComponent() const
        {
            AP_CORE_ASSERT(*this, "Entity is not valid");
            return m_reg->has<Component>(m_handle);
        }

        explicit operator bool() const { return m_handle != entt::null && m_reg; }

#ifdef AP_DEBUG
        /**
         * Do not use this handle for any logical purposes
         *
         * It only server for debug purposes. It represent the entt handle. This
         * handle is unique on each machine and used to communicate with the EnTT API
         */
        entt::entity GetHandle() const {return m_handle;}
#endif

    private:
        entt::entity m_handle = entt::null;
        entt::registry* m_reg = nullptr;
    };
}