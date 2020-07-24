#pragma once
#include "Shinobu/Core/Application.h"
#include "Shinobu/ECS/Scene.h"

inline void DrawSceneStats(sh::Scene& scene)
{
    auto& reg = scene.GetRegistry().Get();
    ImGui::Text("Simulation %i (%i/%i)", 
        scene.GetSimulationCount(), 
        scene.GetCurrentSimulationIndex(), 
        scene.maxSimulations);
    ImGui::Text("Entities %i", reg.size());
}
