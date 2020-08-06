#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Net/Packet.h"

#include <entt/entt.hpp>

#include <functional>

namespace ap
{
    using Entity = entt::entity;
    constexpr auto& NullEntity = entt::null;

    class Registry;

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
    bool UnpackAndReconcileFn(Registry& reg, Entity e, Packet& packet)
    {
        auto& r = reg.Get();
        if (!r.has<T>(e)) r.emplace<T>(e);

        //r.get<T>(e) = Deserialize<T>(packet);
        // Check if we have to correct (client side prediction)
        auto& currentData = r.get<T>(e);
        auto& newData = Deserialize<T>(packet);

        if (currentData == newData)
        {
            return false;
        }
        currentData = newData;
        return true;
    }

    template<typename T>
    void UnpackFn(Registry& reg, Entity e, Packet& packet)
    {
        auto& r = reg.Get();
        if (!r.has<T>(e)) r.emplace<T>(e);

        r.get<T>(e) = Deserialize<T>(packet);
    }

    /**
     * The registry stores entities and their components
     *
     * TODO: Wrap entt instead of directly accessing it
     */
    class APHELION_API Registry
    {
    public:
        using StampFunc = std::function<void(
            const entt::registry& from, const entt::entity src,
            entt::registry& to, const entt::entity dst)>;
        using UnpackFunc = std::function<void(Registry& reg, Entity e, Packet& packet)>;
        using UnpackAndReconcileFunc = std::function<bool(Registry& reg, Entity e, Packet& packet)>;

        using EntityCb = std::function<void(Entity)>;

        struct CompData
        {
            std::string_view name;
            StampFunc stamp;
            UnpackFunc unpack;
            UnpackAndReconcileFunc unpackAndReconcile;
        };

    public:
        // Expose underlying registry, these should not be used but I've added
        // them for the time being so that I can abstract them later
        inline entt::registry& Get() { return m_reg; }
        inline const entt::registry& Get() const { return m_reg; }

        /**
         * Asserts if the hint entity can't be created
         */
        Entity Create();
        Entity Create(Entity hint);

        void Destroy(Entity entity);

        /**
         * Returns true if reconciliation took place
         */
        void HandlePacket(Entity entity, Packet& packet);
        bool HandleAndReconcilePacket(Entity entity, Packet& packet);
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

        void SetOnEntityDestroyCb(EntityCb cb) { m_onCreate = cb; } 
        void SetOnEntityCreateCb(EntityCb cb) { m_onDestroy = cb; }

#ifdef AP_DEBUG
        // Should be used ONLY for debugging purposes
        static std::unordered_map<entt::id_type, CompData>& GetComponentData() { return m_compData; }
#endif

    private:
        static std::unordered_map<entt::id_type, CompData> m_compData;

        EntityCb m_onCreate;
        EntityCb m_onDestroy;
        
        entt::registry m_reg;
    };
}