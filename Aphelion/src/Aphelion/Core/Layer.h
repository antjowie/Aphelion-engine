#pragma once
/**
 * A layer is a interface that is used for rendering and logic
 *
 * Application holds a LayerStack that gets executed on every frame. A layer could be
 * A debug overlay or the game world itself. It is used so that we can easily define the render order
 * and propogate events through it. 
 */

#include "Aphelion/Core/Core.h"
#include "Aphelion/Event/Event.h"
#include "Aphelion/Core/Time.h"

namespace ap
{
    /**
     * The Layer interface, all Layers are to be inherited from this one
     */
    class APHELION_API Layer
    {
    public:
        Layer(const std::string& name = "Layer")
            : m_debugName(name) {}
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnEvent(Event& event) {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnGuiRender() {}
        inline const std::string& GetName() const { return m_debugName; }

    private:
        std::string m_debugName;
    };
}