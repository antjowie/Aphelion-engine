#pragma once
/**
 * The ImGui layer handles basic GUI that will interact with the ECS
 */
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Layer.h"

#include <imgui.h>
//#include <imgui_internal.h>

namespace ap
{
    class APHELION_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();

        /**
         * From imgui.h
         * 1) Important: globals are not shared across DLL boundaries! If you use DLLs or any form of hot-reloading: you will need to call
         *    SetCurrentContext() (with the pointer you got from CreateContext) from each unique static/DLL boundary, and after each hot-reloading.
         *    In your debugger, add GImGui to your watch window and notice how its value changes depending on which location you are currently stepping into.
         */
        static ImGuiContext* GetContext();

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
        //virtual void OnGuiRender() override final;
    };
}