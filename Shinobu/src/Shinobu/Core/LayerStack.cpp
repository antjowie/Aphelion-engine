#include "Shinobu/Core/LayerStack.h"

#include "Shinobu/Core/Log.h"

namespace sh
{
    LayerStack::~LayerStack()
    {
        for (auto layer : m_layers)
        {
            layer->OnDetach();
            delete layer;
        }
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        m_layers.emplace(m_layers.begin() + m_layerIndex, layer);
        layer->OnAttach();
        m_layerIndex++;
        SH_CORE_TRACE("Pushed layer {0}. Current layer count ({1})", layer->GetName(), m_layerIndex);
    }

    void LayerStack::PushOverlay(Layer* overlay)
    {
        m_layers.emplace_back(overlay);
        overlay->OnAttach();
        SH_CORE_TRACE("Pushed overlay {0}. Current overlay count ({1})", overlay->GetName(), m_layers.size() - m_layerIndex);
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        auto result = std::find(begin(), begin() + m_layerIndex, layer);
        if (result != begin() + m_layerIndex)
        {
            layer->OnDetach();
            m_layers.erase(result);
            m_layerIndex--;
            SH_CORE_TRACE("Popped layer {0}. Current layer count ({1})", layer->GetName(), m_layerIndex);
        }
        else
        {
            SH_CORE_WARN("Tried to remove layer {0} but could not find it", layer->GetName());
        }
    }

    void LayerStack::PopOverlay(Layer* overlay)
    {
        auto result = std::find(begin() + m_layerIndex, end(), overlay);
        if (result != end())
        {
            overlay->OnDetach();
            m_layers.erase(result);
            SH_CORE_TRACE("Popped overlay {0}. Current overlay count ({1})", overlay->GetName(), m_layers.size() - m_layerIndex);
        }
        else
        {
            SH_CORE_WARN("Tried to remove overlay {0} but could not find it", overlay->GetName());
        }
    }
}