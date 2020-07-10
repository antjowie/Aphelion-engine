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
     * The ECS is the gameplay system that this engine uses. 
     * It is globally accessible and updates by the gameplay layer
     */
    class ECS
    {
    public:
        /**
         * The registry stores entities and their components
         * 
         * TODO: Wrap entt instead of directly accessing it
         */
        class Registry
        {
        public:
            entt::registry& Get();

        private:
            entt::registry m_reg;
        };

        static Registry& GetRegistry() { return m_reg; };

        /**
         * Be sure to register components. This is needed for serializing component data
         *
         * NOTE: Current implementation relies on entt type id. It is said that this is consistent across boundaries
         * (hopefully meaning that every machine allocates the same component ID). I have not however verified this.
         */
        template <typename T>
        static void RegisterComponent(T t)
        {
            auto id = entt::type_info<T>::id;
            SH_CORE_ASSERT(m_cloneFns.count(id) == 0, "Component has already been registered");
            //m_cloneFns[id] = Clone<T>;
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

        using CloneFunc = std::function<void(entt::registry& from, entt::registry& to)>;
        using SystemFunc = std::function<void(Registry& reg)>;
    private:

        static std::unordered_map<entt::id_type, CloneFunc> m_cloneFns;
        static std::vector<SystemFunc> m_systems;
        static Registry m_reg;
    };

}