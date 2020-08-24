#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Event/Event.h"

namespace ap
{
    /**
     * This function is the function that the Window forwards the events to. 
     */
    using EventCallbackFn = std::function<void(Event&)>;

    /**
     * The WindowProps struct contains all the data consistent among all window classes
     */
    struct WindowProps
    {
    public:
        std::string title;
        unsigned width;
        unsigned height;
        EventCallbackFn eventCallback;

        WindowProps(const std::string& title = "Aphelion Engine",
            unsigned width = 1280,
            unsigned height = 720)
            : title(title), width(width), height(height) {}
    };

    /**
     * The window interface. Implementations of this interface are platform specific
     * and thus defined under the Platform folder
     */
    class APHELION_API Window
    {
    public:
        virtual ~Window() = default;

        virtual unsigned GetWidth() const = 0;
        virtual unsigned GetHeight() const = 0;

        /**
         * Updates the buffers and polls the events
         */
        virtual void OnUpdate() = 0;

        virtual void SetVSync(bool enable) = 0;
        
        /**
         * Application owns a window. The window needs to send events to the application. To do this,
         * Applications gives the Window an event callback function. 
         */
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        /**
         * Return the native window
         * 
         * This is required to set up the ImGui renderer
         */
        virtual void* GetNativeWindow() = 0; 

        /**
         * We override the static create function so that we won't have to bother with 
         * checking the platform in other code
         */
        static std::unique_ptr<Window> Create(WindowProps props = WindowProps());
    };
}