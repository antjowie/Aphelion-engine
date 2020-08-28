#pragma once
#include "Aphelion/Core/Core.h"

#include <entt/entt.hpp>

namespace ap
{
    class APHELION_API Entity;

    /**
     * ComponentCb gets called when the entity makes or removes a component
     * It passes it's own id as well as the compnent id to the registry
     * 
     * The registry then forwards this information to another callback that the user can set up per component
     * The callbacks are stored in the CompData structure in Registry
     */
    using ComponentCb = std::function<void(Entity handle, unsigned compID)>;

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
        Entity(const entt::entity handle, entt::registry& registry, ComponentCb createCb, ComponentCb removeCb)
            : m_handle(handle)
            , m_reg(&registry)
            , m_createCb(createCb)
            , m_removeCb(removeCb)
        {}

        template <typename Component, typename... Args>
        Component& AddComponent(Args&&... args)
        {
            AP_CORE_ASSERT(*this, "Entity is not valid");
            auto& comp = m_reg->emplace<Component>(m_handle, std::forward<Args>(args)...);
            m_createCb(*this, entt::type_info<Component>::id());
            return comp;
        }

        template <typename Component>
        void RemoveComponent()
        {
            AP_CORE_ASSERT(*this, "Entity is not valid");
            AP_CORE_ASSERT(m_reg->has<Component>(m_handle), "Entity does not have that component");
            m_removeCb(*this, entt::type_info<Component>::id());
            m_reg->remove<Component>(m_handle);
        }

        template <typename Component>
        Component& GetComponent()
        {
            AP_CORE_ASSERT(*this, "Entity is not valid");
            AP_CORE_ASSERT(HasComponent<Component>(), "Entity does not have the component");
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
        entt::entity GetHandle() const { return m_handle; }
#endif

    private:
        entt::entity m_handle = entt::null;
        entt::registry* m_reg = nullptr;
        ComponentCb m_createCb;
        ComponentCb m_removeCb;
    };
}