#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Layer.h"
#include "Shinobu/ECS/ECS.h"

namespace sh
{
    /**
     * The ECS layer updates the gameplay system
     * The client can hook into the gameplay system to define cusom behavior
     * 
     * It takes care of updating the ECS. For example, we want a different update frequency when we update the physics.
     * ECSLayer takes care of that
     */
    class SHINOBU_API ECSLayer : public Layer
    {
    public:
        //ECSLayer();
        //void Begin();
        //void End();

        //virtual void OnAttach() override final;
        //virtual void OnDetach() override final;
        //virtual void OnEvent(Event& event) override final;
        virtual void OnUpdate(Timestep dt) override final
        {
            ECS::UpdateSystems();
        }
        //virtual void OnGuiRender() override final;
    };
}