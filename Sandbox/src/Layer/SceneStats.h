#pragma once
#include "Aphelion/Core/Application.h"
#include "Aphelion/ECS/Scene.h"

inline void DrawSceneStats(ap::Scene& scene)
{
    auto& reg = scene.GetRegistry();
    ImGui::Text("Simulation %i (%i/%i)", 
        scene.GetSimulationCount(), 
        scene.GetCurrentSimulationIndex(), 
        scene.maxSimulations);
    ImGui::Text("Entities %i", reg.Count());
}
