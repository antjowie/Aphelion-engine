#pragma once
/**
 * This header contains all the Application type events
 */
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Event/Event.h"

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

    /// Upper left screen coordinates
    class APHELION_API WindowMoveEvent : public Event
    {
    public:
        WindowMoveEvent(int x, int y)
            : m_x(x)
            , m_y(y) {}

        inline int GetX() const { return m_x; }
        inline int GetY() const { return m_y; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowMoveEvent: " << m_x << ", " << m_y;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::WindowMove)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        int m_x;
        int m_y;
    };

    class APHELION_API WindowIconifyEvent : public Event
    {
    public:
        WindowIconifyEvent(bool iconified)
            : m_iconified(iconified) {}

        inline bool IsIconified() const { return m_iconified; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowIconifiedEvent: " << m_iconified;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::WindowIconify)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        bool m_iconified;
    };

    class APHELION_API WindowFocusEvent : public Event
    {
    public:
        WindowFocusEvent(bool focused)
            : m_focused(focused) {}

        inline bool IsFocused() const { return m_focused; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowFocusedEvent: " << m_focused;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::WindowFocus)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        bool m_focused;
    };

} // namespace ap
