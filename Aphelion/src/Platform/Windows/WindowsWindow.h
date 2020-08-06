#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Window.h"

#include "Aphelion/Renderer/GraphicsContext.h"

#include <memory>

struct GLFWwindow;

namespace ap
{
    class APHELION_API WindowsWindow : public Window
    {
    public:
        WindowsWindow(WindowProps& props);
        virtual ~WindowsWindow() override final;

        virtual unsigned GetWidth() const override final { return m_props.width; }
        virtual unsigned GetHeight() const override final { return m_props.height; }

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