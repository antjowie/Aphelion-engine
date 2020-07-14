#pragma once
/**
 * This header has all the mouse related events
 */

#include "Shinobu/Core/Core.h"
#include "Shinobu/Event/Event.h"

#include <string>

namespace sh
{
        //ClientConnectRequest, ClientConnectResponse,
        //ClientSendPacket, ClientReceivePacket,
        //ServerHostRequest, ServerHostResponse,
        //ServerSendPacket, ServerReceivePacket,

    class SHINOBU_API ClientConnectRequestEvent : public Event
    {
    public:
        ClientConnectRequestEvent(const std::string& ip)
            : m_ip(ip) {}

        inline const std::string& GetIP() const { return m_button; }
        
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ClientConnectRequestEvent: ip " << m_ip;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ClientConnectRequest)
        EVENT_CLASS_CATEGORY(EventCategoryNet)

    private:
        std::string m_ip;
    };

    class SHINOBU_API ClientConnectResponseEvent : public Event
    {
    public:
        ClientConnectResponseEvent(const std::string& ip)
            : m_ip(ip) {}

        inline const std::string& GetIP() const { return m_button; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ClientConnectResponseEvent: ip " << m_ip;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ClientConnectRequest)
            EVENT_CLASS_CATEGORY(EventCategoryNet)

    private:
        std::string m_ip;
    };
}