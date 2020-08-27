#include "Platform/Windows/WindowsInput.h"

#include "Aphelion/Core/Application.h"

#include <GLFW/glfw3.h>

namespace ap
{
    GLFWwindow* GetNativeWindow()
    {
        return static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    }

    Window& GetWindow()
    {
        return Application::Get().GetWindow();
    }

    void WindowsInput::EnableCursorImpl(bool enable)
    {
        glfwSetInputMode(GetNativeWindow(), GLFW_CURSOR, enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    bool WindowsInput::IsKeyPressedImpl(KeyCode code)
    {
        return glfwGetKey(GetNativeWindow(), static_cast<int>(code));
    }

    bool WindowsInput::IsButtonPressedImpl(ButtonCode code) 
    {
        return glfwGetMouseButton(GetNativeWindow(), static_cast<int>(code));
    }

    void WindowsInput::SetCursorPosImpl(const glm::vec2& pos)
    {
        glfwSetCursorPos(GetNativeWindow(), pos.x, pos.y);
    }

    void WindowsInput::SetCursorXImpl(float x)
    {
        glfwSetCursorPos(GetNativeWindow(), x, GetCursorYImpl());
    }
    
    void WindowsInput::SetCursorYImpl(float y)
    {
        glfwSetCursorPos(GetNativeWindow(), GetCursorXImpl(), y);
    }

    glm::dvec2 WindowsInput::GetCursorPosImpl() 
    {
        glm::dvec2 ret;
        glfwGetCursorPos(GetNativeWindow(), &ret.x, &ret.y);
        return ret;
    }
    
    float WindowsInput::GetCursorXImpl() 
    {
        return GetCursorPosImpl().x;
    }
    
    float WindowsInput::GetCursorYImpl() 
    {
        return GetCursorPosImpl().y;
    }
}