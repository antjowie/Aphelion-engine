#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Window.h"

#include "Shinobu/Renderer/GraphicsContext.h"

#include <memory>

struct GLFWwindow;

namespace sh
{
    class SHINOBU_API WindowsWindow : public Window
    {
    public:
        WindowsWindow(WindowProps& props);
        virtual ~WindowsWindow() override final;

        virtual void OnUpdate() override final;
        virtual void SetVSync(bool enable) override final;

        inline virtual void SetEventCallback(const EventCallbackFn& callback) override final { m_props.eventCallback = callback; }

        inline virtual void* GetNativeWindow() override final { return m_window; }

    private:
        std::unique_ptr<GraphicsContext> m_context;
        GLFWwindow* m_window;
        WindowProps m_props;
    };
}