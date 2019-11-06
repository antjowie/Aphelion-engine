#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Event/Event.h"

namespace sh
{
    class SHINOBU_API WindowCloseEvent : public Event
    {
    public:
        EVENT_CLASS_TYPE(EventType::WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class SHINOBU_API WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(unsigned width, unsigned height)
            : m_width(width)
            , m_height(height)
        {
        }

        inline unsigned GetWidth() const { return m_width; }
        inline unsigned GetHeight() const { return m_height; }

        std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}

        EVENT_CLASS_TYPE(EventType::WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        unsigned m_width;
        unsigned m_height;
    };

    class SHINOBU_API AppTickEvent : public Event
    {
    public:
        EVENT_CLASS_TYPE(EventType::AppTick)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class SHINOBU_API AppUpdateEvent : public Event
    {
    public:
        EVENT_CLASS_TYPE(EventType::AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class SHINOBU_API AppRenderEvent : public Event
    {
    public:
        EVENT_CLASS_TYPE(EventType::AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
} // namespace sh
