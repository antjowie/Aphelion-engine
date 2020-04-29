#pragma once
/**
 * The ImGui layer handles basic GUI that will interact with the ECS
 */
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Layer.h"

namespace sh
{
    class SHINOBU_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();

        /**
         * Sets up ImGui, should be called every frame before any ImGui calls are made 
         */
        void Begin();
        /**
         * Sets up ImGui, should be called every frame before any ImGui calls are made 
         */
        void End();

        virtual void OnAttach() override final;
        virtual void OnDetach() override final;
        //virtual void OnEvent(Event& event) override final;
        //virtual void OnUpdate() override final;
        virtual void OnGuiRender() override final;
    };
}