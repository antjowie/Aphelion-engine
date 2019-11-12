#include "Shinobu/ImGui/ImGuiLayer.h"

#include <imgui.h>

namespace sh
{
    ImGuiLayer::ImGuiLayer()
        : Layer("ImGuiLayer") {}


    void ImGuiLayer::OnAttach()
    {

    }

    void ImGuiLayer::OnDetach()
    {

    }

    void ImGuiLayer::OnUpdate()
    {
        static bool show = true;
        ImGui::ShowDemoWindow(&show);
    }

    void ImGuiLayer::OnEvent(Event& event)
    {
    }

}