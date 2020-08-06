#include "Aphelion/Core/LayerStack.h"

#include "Aphelion/Core/Log.h"

namespace ap
{
    LayerStack::LayerStack()
    {
        m_layers.reserve(6);
    }
    
    LayerStack::~LayerStack()
    {
        Clear();
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        SH_CORE_TRACE("Pushed layer {0}. Current layer count ({1})", layer->GetName(), m_layerIndex);
        m_layers.emplace(m_layers.begin() + m_layerIndex, layer);
        layer->OnAttach();
        m_layerIndex++;
        AP_CORE_ASSERT(m_layers.size() <= 6, "Can't push more than 6 layers. Otherwise the vector resizes");
    }

    void LayerStack::PushOverlay(Layer* overlay)
    {
        SH_CORE_TRACE("Pushed overlay {0}. Current overlay count ({1})", overlay->GetName(), m_layers.size() - m_layerIndex);
        m_layers.emplace_back(overlay);
        overlay->OnAttach();
        AP_CORE_ASSERT(m_layers.size() <= 6, "Can't push more than 6 layers. Otherwise the vector resizes");
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
    
    void LayerStack::Clear()
    {
        if (m_layers.empty()) return;

        //SH_CORE_INFO("Clearing layers...");
        //for (auto layer : ap::Reverse(m_layers))
        //{
        //    const auto& name = layer->GetName();
        //    SH_CORE_TRACE("Popping layer {}", name);
        //    layer->OnDetach();
        //    delete layer;
        //    SH_CORE_TRACE("Popped layer {}", name);
        //}
        //m_layers.clear();

        SH_CORE_INFO("Clearing layers...");
        for (auto i = m_layers.size() - 1;; i--)
        {
            auto* layer = m_layers[i];
            auto name = layer->GetName();
            SH_CORE_TRACE("Popping layer {}", name);
            layer->OnDetach();
            m_layers.pop_back();
            delete layer;
            SH_CORE_TRACE("Popped layer {}", name);

            if (i == 0) break;
        }

        m_layerIndex = 0;
    }
}