#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/PRNG.h"
#include "Aphelion/Net/Packet.h"
#include "Aphelion/ECS/Entity.h"
#include "Aphelion/ECS/Component.h"

#include <entt/entt.hpp>

#include <functional>
#include <vector>
#include <unordered_map>

namespace ap
{
    /**
     * A function to stamp an component value of an entity to another entity
     */
    template<typename T>
    void StampFn(const entt::registry& from, const entt::entity src, entt::registry& to, const entt::entity dst) 
    {
        to.emplace_or_replace<T>(dst, from.get<T>(src));
    }

    /**
     * Unpacks a packet and sets the component value to the packet value
     * If the registry value differs from the packet value, returns false meaning the 
     * registry should be reconciled
     * TODO: This couples the ECS to our netcode. The ECS should not bother thinking about netcode
     * Or maybe it should since it needs to store a function to extract the data
     *
     * reg is the registry rolled back to the simulation in which packet was created
     * returns true if reconciliation took place
     */
    template<typename T>
    bool UnpackAndReconcileFn(entt::registry& reg, entt::entity handle, Packet& packet)
    {
        auto& r = reg;
        if (!r.has<T>(handle)) r.emplace<T>(handle);

        //r.get<T>(e) = Deserialize<T>(packet);
        // Check if we have to correct (client side prediction)
        auto& currentData = r.get<T>(handle);
        auto& newData = Deserialize<T>(packet);

        if (currentData == newData)
        {
            return true;
        }
        currentData = newData;
        return false;
    }

    template<typename T>
    void UnpackFn(entt::registry& reg, entt::entity handle, Packet& packet)
    {
        reg.get_or_emplace<T>(handle) = Deserialize<T>(packet);

        //if (!r.has<T>(handle)) r.emplace<T>(handle);
        //r.get<T>(handle) = Deserialize<T>(packet);
    }

    /**
     * Taken from EnTT
     * Reg View takes 2 variadic arguments
     * Since variadic arguments are gready, we pass the second set via a pack of 
     * types, hence typelist. 
     */
    template <typename... T>
    struct TypeList {};

    /**
     * Allows us to nicely make a list using the following syntax
     * typeList<SomeComponent>
     */
    template <typename... T>
    inline constexpr TypeList<T...> typeList{};

    /**
     * The registry stores entities and their components
     */
    class APHELION_API Registry
    {
    public:
        using StampFunc = std::function<void(
            const entt::registry& from, const entt::entity src,
            entt::registry& to, const entt::entity dst)>;
        using UnpackFunc = std::function<void(entt::registry& reg, entt::entity handle, Packet& packet)>;
        using UnpackAndReconcileFunc = std::function<bool(entt::registry& reg, entt::entity handle, Packet& packet)>;

        using EntityCb = std::function<void(Entity)>;
        
        struct CompData
        {
            std::string_view name;
            StampFunc stamp;
            UnpackFunc unpack;
            UnpackAndReconcileFunc unpackAndReconcile;
            EntityCb createComp;
            EntityCb removeComp;
        };

    public:
        Entity Create(const std::string& tag = {});
        /// Used when recreating an existing entity (from another source such as network for example)
        Entity Create(unsigned guid, const std::string& tag = {});
        void Destroy(Entity entity);
        Entity Get(unsigned guid);
        Entity Get(entt::entity handle);
        bool Has(unsigned guid) const { return m_idToHandle.count(guid) == 1; }

        size_t Count() const { return m_reg.size(); }

        template<typename... Component, typename... Exclude, typename CB>
        void View(CB& callback, TypeList<Exclude...> = {}) {
            auto view = m_reg.view<Component...>(entt::exclude<Exclude...>);
            auto cbWrap = [&](entt::entity entity, auto&... param) 
            { callback(Get(entity), param...); };

            view.each(cbWrap);

            //static_assert(sizeof...(Component) > 0, "Exclusion-only views are not supported");
            //return { assure<std::decay_t<Component>>()..., assure<Exclude>()... };
        }

        /**
         * Returns true if reconciliation took place
         * TODO: Refactor this so that registry no longer depends on Net system
         */
        void HandlePacket(unsigned guid, Packet& packet);
        bool HandleAndReconcilePacket(unsigned guid, Packet& packet);
        void Clone(Registry& from);

        /**
         * Be sure to register components.
         * Registering components allows us to serialize the components and
         * it allows us to store metadata such as the name of the component
         *
         * NOTE: Current implementation relies on entt type id. It is said that this is consistent across boundaries
         * (hopefully meaning that every machine allocates the same component ID). I have not however verified this.
         */
        template <typename T>
        static void RegisterComponent()
        {
            auto id = entt::type_info<T>::id();
            AP_CORE_ASSERT(m_compData.count(id) == 0, "Component has already been registered");
            m_compData[id].stamp = StampFn<T>;
            m_compData[id].unpack = UnpackFn<T>;
            m_compData[id].unpackAndReconcile = UnpackAndReconcileFn<T>;
            m_compData[id].name = entt::type_info<T>::name();
        }

        void SetOnEntityDestroyCb(EntityCb cb)  { m_onCreate = cb; }
        void SetOnEntityCreateCb(EntityCb cb) { m_onDestroy = cb; }

#ifdef AP_DEBUG
        // Should be used ONLY for debugging purposes
        static std::unordered_map<entt::id_type, CompData>& GetComponentData() { return m_compData; }
#endif

    private:
        static std::unordered_map<entt::id_type, CompData> m_compData;
        static PRNG m_prng;

        std::unordered_map<unsigned, entt::entity> m_idToHandle;

        EntityCb m_onCreate;
        EntityCb m_onDestroy;
        
        entt::registry m_reg;
    };
}