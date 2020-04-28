#pragma once
/**
 * This header contains all the Layer type events
 */
#include "Shinobu/Event/Event.h"

namespace sh
{

    class SHINOBU_API LayerUpdateEvent : public Event
    {
    public:
        EVENT_CLASS_TYPE(EventType::LayerUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryLayer)
    };

    class SHINOBU_API LayerGuiRenderEvent: public Event
    {
    public:
        EVENT_CLASS_TYPE(EventType::LayerGuiRender)
        EVENT_CLASS_CATEGORY(EventCategoryLayer)
    };

    class SHINOBU_API LayerRenderEvent: public Event
    {
    public:
        EVENT_CLASS_TYPE(EventType::LayerRender)
        EVENT_CLASS_CATEGORY(EventCategoryLayer)
    };
}