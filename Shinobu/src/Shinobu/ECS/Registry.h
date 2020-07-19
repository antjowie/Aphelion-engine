#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Time.h"

#include <entt/entt.hpp>

#include <vector>
#include <functional>

// Coupled to packet. Gotta do something about this
// Or should I? Is there any way to get the template argument during
// runtime?
#include "Shinobu/Net/Packet.h"

namespace sh
{
    using Entity = entt::entity;
    class Registry;

    /**
     * A function to clone components from one registry to another
     */
    template<typename T>
    void CloneFn(const entt::registry& from, entt::registry& to) {
        //const auto* data = from.data<T>();
        //const auto size = from.size<T>();

        //if constexpr (ENTT_IS_EMPTY(T)) {
        //    // This probably crashed but haven't tested it yet
        //    to.insert<T>(data, data + size);
        //}
        //else {
        //    // Iterate over each entity in from registry
        //    from.each([&](const entt::entity e)
        //        {
        //            if (!to.has(e)) SH_VERIFY(to.create(e) == e, "Could not copy entity from registry");

        //            // Copy each entity into the 
        //            from.visit([&](const entt::id_type component)
        //                {
        //                    to.emplace_or_replace<T>(dst, from.get<T>(src));
        //                })
        //        });

        //    to.emplace_or_replace
        //    const auto* raw = from.raw<T>();

        //    // Clear this reg
        //    to.
        //    // Fill with needed entities
        //    auto* toData = to.data<T>();
        //    to.create(toData, toData + size);
        //    to.insert<T>(data, data + size, raw, raw + size);
        //}
    }

    /**
     * A function to stamp an component value of an entity to another entity
     */
    template<typename T>
    void StampFn(const entt::registry& from, const entt::entity src, entt::registry& to, const entt::entity dst) 
    {
        to.emplace_or_replace<T>(dst, from.get<T>(src));
    }

    /**
     * Unpacks a packet
     * TODO: This couples the ECS to our netcode. The ECS should not bother thinking about netcode
     * Or maybe it should since it needs to store a function to extract the data
     */
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
    class SHINOBU_API Registry
    {
    public:

        using CloneFunc = std::function<void(const entt::registry& from, entt::registry& to)>;
        using StampFunc = std::function<void(
            const entt::registry& from, const entt::entity src,
            entt::registry& to, const entt::entity dst)>;
        using UnpackFunc = std::function<void(Registry& reg, Entity e, Packet& packet)>;

        struct CompData
        {
            std::string_view name;
            CloneFunc clone;
            StampFunc stamp;
            UnpackFunc unpack;
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

        void HandlePacket(Entity entity, Packet& packet);
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
            SH_CORE_ASSERT(m_compData.count(id) == 0, "Component has already been registered");
            m_compData[id].clone = CloneFn<T>;
            m_compData[id].stamp = StampFn<T>;
            m_compData[id].unpack = UnpackFn<T>;
            m_compData[id].name = entt::type_info<T>::name();
        }

#ifdef SH_DEBUG
        // Should be used ONLY for debugging purposes
        const std::unordered_map<entt::id_type, CompData>& GetComponentData() const { return m_compData; }
#endif

    private:
        static std::unordered_map<entt::id_type, CompData> m_compData;
        
        entt::registry m_reg;
    };
}