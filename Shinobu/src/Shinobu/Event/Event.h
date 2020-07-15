#pragma once
/**
 * This header contains the event base class that all events inherit from
 */

#include "Shinobu/Core/Core.h"

namespace sh
{
    enum class EventType
    {
        None = 0,
        // Window events
        WindowClose, WindowResize,

        // Input events
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, 
        MouseMoved, MouseScrolled,

        // Net event
        ClientConnectRequest, ClientConnectResponse,
        ClientDisconnectRequest, ClientDisconnectResponse, 
        ClientSendPacket, ClientReceivePacket,
        ServerHostRequest, ServerHostResponse,
        ServerClientConnect, ServerClientDisconnect,
        ServerShutdownRequest, ServerShutdownResponse,
        ServerSendPacket, ServerBroadcastPacket, ServerReceivePacket,
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryApplication    = Bit(0), // Application events are program related events such as shutdown
        EventCategoryInput          = Bit(1), // Input are related to the way user interacts
        EventCategoryKeyboard       = Bit(2), 
        EventCategoryMouse          = Bit(3),
        EventCategoryMouseButton    = Bit(4),
        EventCategoryNet            = Bit(5),
        EventCategoryNetClient      = Bit(6),
        EventCategoryNetServer      = Bit(7),
    };

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return type; } \
                               virtual EventType GetEventType() const override { return GetStaticType(); } \
                               virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return EventCategory::category; }

    class SHINOBU_API Event
    {
    public:
        bool handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        inline bool IsInCategory(EventCategory category) const 
        { 
            return GetCategoryFlags() & category; 
        }
    };

    class SHINOBU_API EventDispatcher
    {
    public:
        EventDispatcher(Event& event)
            : m_event(event)
        {
        }

        /**
         * Dispatches the event if the function type corresponds with T
         * Returns wether the event type is the same as the function type
         */ 
        template <typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_event.GetEventType() == T::GetStaticType())
            {
                m_event.handled = func(static_cast<T&>(m_event));
                return true;
            }
            return false;
        }

    private:
        Event& m_event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }

} // namespace sh
