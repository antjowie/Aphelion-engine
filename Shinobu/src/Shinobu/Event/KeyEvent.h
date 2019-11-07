#pragma once
/**
 * This header has all the Keyboard related events
 */

#include "Shinobu/Core/Core.h"
#include "Shinobu/Event/Event.h"

namespace sh
{ 
    class SHINOBU_API KeyPressedEvent : public Event
    {
    public:
        KeyPressedEvent(int keyCode, unsigned repeatCount)
            : m_keyCode(keyCode)
            , m_repeatCount(repeatCount) {}

        inline int GetKeyCode() const { return m_keyCode; }
        inline unsigned GetRepeatCount() const { return m_repeatCount; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: key " << m_keyCode << " repeat " << m_repeatCount;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::KeyPressed)
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

    private:
        int m_keyCode;
        unsigned m_repeatCount;
    };

    class SHINOBU_API KeyReleasedEvent : public Event
    {
    public:
        KeyReleasedEvent(int keyCode)
            : m_keyCode(keyCode) {}

        inline int GetKeyCode() const { return m_keyCode; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: key " << m_keyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::KeyReleased)
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

    private:
        int m_keyCode;
    };
}