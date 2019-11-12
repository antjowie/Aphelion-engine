#pragma once
/**
 * A layer is a interface that is used for rendering and logic
 *
 * Application holds a LayerStack that gets executed on every frame. A layer could be
 * A debug overlay or the game world itself. It is used so that we can easily define the render order
 * and propogate events through it. 
 */

#include "Shinobu/Core/Core.h"
#include "Shinobu/Event/Event.h"

namespace sh
{
    /**
     * The Layer interface, all Layers are to be inherited from this one
     */
    class SHINOBU_API Layer
    {
    public:
        Layer(const std::string& name = "Layer")
            : m_debugName(name) {}
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnEvent(Event& event) {}

        inline const std::string& GetName() const { return m_debugName; }

    private:
        std::string m_debugName;
    };
}