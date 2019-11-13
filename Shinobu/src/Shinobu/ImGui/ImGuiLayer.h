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

        void Begin();
        void End();

        virtual void OnAttach() override final;
        virtual void OnDetach() override final;
        virtual void OnUpdate() override final;
        virtual void OnEvent(Event& event) override final;
    };
}