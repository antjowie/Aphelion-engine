#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/ECS/Registry.h"

namespace sh
{
    /**
     * A Scene (aka game world) hold our registry and versions of it.
     * This is used to rollback the simulation for net features such as
     * server reconcilliation, client prediction and interpolation
     */
    class SHINOBU_API Scene
    {
    public:
        constexpr static unsigned maxRegistries = 256;
        using SystemFunc = std::function<void(Scene& scene)>;

    public:
        Registry& GetRegistry(unsigned offset = 0);

        unsigned GetSimulationCount() const;

        void Simulate(Timestep ts);

        /**
         * You can register systems here so you don't have to update them yourself.
         * Of course, nothing is stopping you from doing so. But the ECS will in the future
         * parallize read only systems.
         *
         * T should be a callable
         */
        template <typename T>
        void RegisterSystem(T&& t)
        {
            static_assert(std::is_invocable_v<T, Scene&>,
                "T should be callable, make sure operator() is overloaded with argument Registry&");
            m_systems.push_back(std::forward<T>(t));
        }

        void ClearSystems();

    private:
        Registry m_registries[maxRegistries];
        unsigned m_currentRegistry = 0;
        unsigned m_simulationCount = 0;

        std::vector<SystemFunc> m_systems;
    };
}