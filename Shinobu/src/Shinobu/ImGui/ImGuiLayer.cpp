#include "Shinobu/ImGui/ImGuiLayer.h"

#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

// Temporarily 
#include <GLFW/glfw3.h>

namespace sh
{
    ImGuiLayer::ImGuiLayer()
        : Layer("ImGuiLayer") 
    {
        SH_CORE_INFO("Running ImGui {0}", IMGUI_VERSION);
    }

    void Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void End()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

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