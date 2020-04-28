#pragma once
/**
 * The Windows implementation of Window
 */
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Window.h"
#include "Shinobu/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace sh
{
    class SHINOBU_API WindowsWindow : public Window
    {
    public:
        WindowsWindow(WindowProps& props);
        virtual ~WindowsWindow() override;

        virtual void OnUpdate() override;
        virtual void SetVSync(bool enable) override;

        inline virtual void SetEventCallback(const EventCallbackFn& callback) override { m_props.eventCallback = callback; }

        inline virtual void* GetNativeWindow() override { return m_window; }

    private:
        std::unique_ptr<GraphicsContext> m_context;
        GLFWwindow* m_window;
        WindowProps m_props;
    };
}