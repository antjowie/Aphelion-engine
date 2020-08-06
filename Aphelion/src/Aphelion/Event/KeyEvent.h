#pragma once
/**
 * This header has all the Keyboard related events
 */

#include "Aphelion/Core/Core.h"
#include "Aphelion/Event/Event.h"

namespace ap
{
    class APHELION_API KeyEvent : public Event
    {
    public:
        KeyEvent(int keyCode)
            : m_keyCode(keyCode) {}

        inline int GetKeyCode() const { return m_keyCode; }

        std::string ToString() const override
        {
            std::stringstream ss;
            
            ss << GetName() << ": key " << m_keyCode;
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

    protected:
        int m_keyCode;
    };


    class APHELION_API KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(int keyCode, unsigned repeatCount)
            : KeyEvent(keyCode)
            , m_repeatCount(repeatCount) {}

        inline unsigned GetRepeatCount() const { return m_repeatCount; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: key " << m_keyCode << " repeat " << m_repeatCount;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::KeyPressed)

    private:
        unsigned m_repeatCount;
    };

    class APHELION_API KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(int keyCode)
            : KeyEvent(keyCode) {}

        EVENT_CLASS_TYPE(EventType::KeyReleased)
    };
        
    /**
     * Returns a typed key
     * 
     * According to the <a href="https://www.glfw.org/docs/latest/group__input.html#ga556239421c6a5a243c66fca28da9f742">
     * GLFW documentation</a> we have a character callback. Characters can be 
     * more than one character so the KeyPressedEvent doesn't suffice
     */
    class APHELION_API KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(int keyCode)
            : KeyEvent(keyCode) {}

        EVENT_CLASS_TYPE(EventType::KeyTyped)
    };
}