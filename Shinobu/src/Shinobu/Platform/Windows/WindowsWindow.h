#pragma once
/**
 * The Windows implementation of Window
 */
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Window.h"

namespace sh
{
    class SHINOBU_API WindowsWindow : public Window
    {
    public:
        WindowsWindow(WindowProps& props);
        
        virtual void OnUpdate() override;

        inline virtual void SetEventCallback(const EventCallbackFn& callback) override { m_props.eventCallback = callback; }

    private:
        WindowProps m_props; 
    };
}