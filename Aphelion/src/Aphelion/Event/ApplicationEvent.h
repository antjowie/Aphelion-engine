#pragma once
/**
 * This header contains all the Application type events
 */
#include "Aphelion/Core/Core.h"
#include "Aphelion/Event/Event.h"

namespace ap
{
    class APHELION_API WindowCloseEvent : public Event
    {
    public:
        EVENT_CLASS_TYPE(EventType::WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class APHELION_API WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(unsigned width, unsigned height)
            : m_width(width)
            , m_height(height) {}

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
} // namespace ap
