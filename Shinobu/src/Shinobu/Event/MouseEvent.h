#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Event/Event.h"

namespace sh
{
    class SHINOBU_API MouseButtonPressedEvent : public Event
    {
    public:
        MouseButtonPressedEvent(unsigned buttonCode, unsigned repeatCount)
            : m_buttonCode(buttonCode)
            , m_repeatCount(repeatCount)
        {
        }

        inline unsigned GetKeyCode() const { return m_buttonCode; }
        inline unsigned GetRepeatCount() const { return m_repeatCount; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: button " << m_buttonCode << " repeat " << m_repeatCount;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseButtonPressed)
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

    private:
        unsigned m_buttonCode;
        unsigned m_repeatCount;
    };

    class SHINOBU_API MouseButtonReleasedEvent : public Event
    {
    public:
        MouseButtonReleasedEvent(unsigned buttonCode, unsigned repeatCount)
            : m_buttonCode(buttonCode)
        {
        }

        inline unsigned GetKeyCode() const { return m_buttonCode; }
        
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: button " << m_buttonCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseButtonReleased)
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

    private:
        unsigned m_buttonCode;
    };
}