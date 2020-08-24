#pragma once
/**
 * A collection of common header files that the client will use
 */ 

#include "Aphelion/Core/Application.h"
#include "Aphelion/Core/EntryPoint.h"
#include "Aphelion/Core/Layer.h"
#include "Aphelion/Core/LayerStack.h"
#include "Aphelion/Core/Log.h"
#include "Aphelion/Core/Time.h"
#include "Aphelion/Core/Transform.h"

#include "Aphelion/Core/Input/Input.h"
#include "Aphelion/Core/Input/KeyCodes.h"
#include "Aphelion/Core/Input/MouseCodes.h"

#include "Aphelion/ECS/Registry.h"

#include "Aphelion/Core/Event/Event.h"
#include "Aphelion/Core/Event/KeyEvent.h"
#include "Aphelion/Core/Event/MouseEvent.h"

#include "Aphelion/Renderer/Renderer.h"
#include "Aphelion/Renderer/Renderer2D.h"
#include "Aphelion/Renderer/OrthographicCamera.h"
#include "Aphelion/Renderer/OrthographicCameraController.h"
#include "Aphelion/Renderer/PerspectiveCamera.h"
#include "Aphelion/Renderer/PerspectiveCameraController.h"
#include "Aphelion/Renderer/Shader.h"
#include "Aphelion/Renderer/Texture.h"

#include "Aphelion/Core/ImGui/ImGuiLayer.h"