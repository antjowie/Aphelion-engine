#pragma once
/**
 * This header has all the mouse related events
 */

#include "Shinobu/Core/Core.h"
#include "Shinobu/Event/Event.h"

namespace sh
{
    class SHINOBU_API MouseButtonPressedEvent : public Event
    {
    public:
        MouseButtonPressedEvent(int button, unsigned repeatCount)
            : m_button(button)
            , m_repeatCount(repeatCount) {}

        inline int GetButton() const { return m_button; }
        inline unsigned GetRepeatCount() const { return m_repeatCount; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressedEvent: button " << m_button << " repeat " << m_repeatCount;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseButtonPressed)
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouseButton)

    private:
        int m_button;
        unsigned m_repeatCount;
    };

    class SHINOBU_API MouseButtonReleasedEvent : public Event
    {
    public:
        MouseButtonReleasedEvent(int button)
            : m_button(button) {}

        inline int GetButton() const { return m_button; }
        
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: button " << m_button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseButtonReleased)
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouseButton)

    private:
        int m_button;
    };

    class SHINOBU_API MouseMovedEvent : public Event
    {
    public:
        MouseMovedEvent(float x, float y)
            : m_x(x)
            , m_y(y) {}

        inline float GetX() const { return m_x; }
        inline float GetY() const { return m_y; }
        inline std::pair<float, float> GetPosition() const { return { GetX(), GetY() }; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: x " << m_x << " y " << m_y;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

    private:
        float m_x;
        float m_y;
    };

    class SHINOBU_API MouseScrolledEvent : public Event
    {
    public:
        MouseScrolledEvent(float xOffset, float Offset)
            : m_xOffset(xOffset)
            , m_yOffset(Offset) {}

        inline float GetXOffset() const { return m_xOffset; }
        inline float GetYOffset() const { return m_yOffset; }
        inline std::pair<float, float> GetScrollOffset() const { return { GetXOffset(), GetYOffset() }; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent: xOffset " << m_xOffset << " yOffset " << m_yOffset;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)

    private:
        float m_xOffset;
        float m_yOffset;
    };
}