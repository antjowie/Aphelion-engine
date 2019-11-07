#pragma once
/**
 * The layer stack is a wrapper around a data structure that holds Layers
 */

#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Layer.h"

namespace sh
{
    /**
     * The LayerStack is a container for layers. It allows the client to easily push layers or overlays 
     * without having to worry about the correct order
     *
     * The LayerStack takes ownership of a layer so you should not delete layers that are passed. 
     * We do this because you can pass a layer that you wan't to remove such as a debug layer. 
     *
     * If you have removed a layer from the LayerStack, you have to deallocate it yourself. 
     */
    class SHINOBU_API LayerStack
    {
    public:
        ~LayerStack();

        /**
         * Adds a layer to the back of the LayerStack
         *
         * Layers are rendered before events but they receive events last
         */
        void PushLayer(Layer* layer);

        /**
         * Adds an overlay to the back of the LayerStack
         *
         * Overlays are rendered after layers but they receive all events first
         */
        void PushOverlay(Layer* overlay);

        /**
         * Removes the passed layer from the LayerStack
         *
         * Be sure to deallocate the pointer yourself since the LayerStack no longer
         * keeps ownership of it
         */
        void PopLayer(Layer* layer);
        
        /**
         * Removes the passed overlay from the LayerStack
         * 
         * Be sure to deallocate the pointer yourself since the LayerStack no longer
         * keeps ownership of it
         */
        void PopOverlay(Layer* overlay);

        inline std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
        inline std::vector<Layer*>::iterator end() { return m_layers.end(); }

    private:
        std::vector<Layer*> m_layers;
        unsigned m_layerIndex = 0;
    };
}