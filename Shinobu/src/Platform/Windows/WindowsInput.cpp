#include "Platform/Windows/WindowsInput.h"

#include "Shinobu/Core/Application.h"

#include <GLFW/glfw3.h>

namespace sh
{
    GLFWwindow* GetWindow()
    {
        return static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    }

    bool WindowsInput::IsKeyPressedImpl(KeyCode code)
    {
        return glfwGetKey(GetWindow(), static_cast<int>(code));
    }

    bool WindowsInput::IsButtonPressedImpl(ButtonCode code) 
    {
        return glfwGetMouseButton(GetWindow(), static_cast<int>(code));
    }

    glm::vec2 WindowsInput::GetMousePosImpl() 
    {
        glm::dvec2 ret;
        glfwGetCursorPos(GetWindow(), &ret.x, &ret.y);
        return ret;
    }
    
    float WindowsInput::GetMouseXImpl() 
    {
        glm::dvec2 ret;
        glfwGetCursorPos(GetWindow(), &ret.x, &ret.y);
        return ret.x;

    }
    
    float WindowsInput::GetMouseYImpl() 
    {
        glm::dvec2 ret;
        glfwGetCursorPos(GetWindow(), &ret.x, &ret.y);
        return ret.y;
    }
}