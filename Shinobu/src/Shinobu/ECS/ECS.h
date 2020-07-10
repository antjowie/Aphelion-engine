#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Time.h"

#include <entt/entt.hpp>

#include <vector>
#include <functional>

namespace sh
{
    /**
     * A function to clone components from one registry to another
     */
    template<typename T>
    void Clone(const entt::registry& from, entt::registry& to) {
        const auto* data = from.data<T>();
        const auto size = from.size<T>();

        if constexpr (ENTT_IS_EMPTY(T)) {
            to.insert<T>(data, data + size);
        }
        else {
            const auto* raw = from.raw<T>();
            to.insert<T>(data, data + size, raw, raw + size);
        }
    }

    /**
     * A function to stamp an component value of an entity to another entity
     */
    template<typename T>
    void Stamp(const entt::registry& from, const entt::entity src, entt::registry& to, const entt::entity dst) 
    {
        to.emplace_or_replace<T>(dst, from.get<T>(src));
    }

    /**
     * The ECS is the gameplay system that this engine uses. 
     * It is globally accessible and updates by the gameplay layer
     */
    class ECS
    {
    public:
        using Entity = entt::entity;

        /**
         * The registry stores entities and their components
         *
         * TODO: Wrap entt instead of directly accessing it
         */
        class Registry
        {
        public:
            // Expose underlying registry, these should not be used but I've added
            // them for the time being so that I can abstract them later
            inline entt::registry& Get() { return m_reg; }
            inline const entt::registry& Get() const { return m_reg; }

            /**
             * Asserts if the hint entity can't be created
             */
            Entity Create(Entity hint);

        private:
            entt::registry m_reg;
        };

        using CloneFunc = std::function<void(entt::registry& from, entt::registry& to)>;
        using StampFunc = std::function<void
        (const entt::registry& from, const entt::entity src, entt::registry& to, const entt::entity dst)>;
        using SystemFunc = std::function<void(Registry& reg)>;

        struct CompData
        {
            std::string_view name;
            CloneFunc clone;
            StampFunc stamp;
        };

    public:
        static Registry& GetRegistry() { return m_reg; };

        /**
         * Be sure to register components. This is needed for serializing component data
         *
         * NOTE: Current implementation relies on entt type id. It is said that this is consistent across boundaries
         * (hopefully meaning that every machine allocates the same component ID). I have not however verified this.
         */
        template <typename T>
        static void RegisterComponent()
        {
            auto id = entt::type_info<T>::id();
            SH_CORE_ASSERT(m_compData.count(id) == 0, "Component has already been registered");
            m_compData[id].clone = Clone<T>;
            m_compData[id].stamp = Stamp<T>;
            m_compData[id].name = entt::type_info<T>::name();
        }

        /**
         * You can register systems here so you don't have to update them yourself. 
         * Of course, nothing is stopping you from doing so. But the ECS will in the future
         * parallize read only systems.
         *
         * T should be a callable 
         */
        template <typename T>
        static void RegisterSystem(T t)
        {
            static_assert(std::is_invocable_v<T, Registry&>, 
                "T should be callable, make sure operator() is overloaded with argument Registry");
            m_systems.push_back(t);
        }

        static void ClearSystems();
        static void UpdateSystems();

        // Should be used for debugging purposes
        static const std::unordered_map<entt::id_type, CompData>& GetComponentData();

    private:
        static std::unordered_map<entt::id_type, CompData> m_compData;
        static std::vector<SystemFunc> m_systems;
        static Registry m_reg;
    };

}