#pragma once
/**
 * The Windows implementation of Window
 */
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Window.h"

struct GLFWwindow;

namespace sh
{
    class SHINOBU_API WindowsWindow : public Window
    {
    public:
        WindowsWindow(WindowProps& props);
        virtual ~WindowsWindow() override;

        virtual void OnUpdate() override;

        inline virtual void SetEventCallback(const EventCallbackFn& callback) override { m_props.eventCallback = callback; }

    private:

        GLFWwindow* m_window;
        WindowProps m_props; 
    };
}