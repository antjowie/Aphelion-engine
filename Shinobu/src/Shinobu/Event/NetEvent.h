#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Event/Event.h"
#include "Shinobu/Net/Packet.h"

#include <string>

namespace sh
{
    ////////////////////////////////////////////
    // Client events
    ////////////////////////////////////////////

    class SHINOBU_API ClientConnectRequestEvent : public Event
    {
    public:
        ClientConnectRequestEvent(const std::string& ip, unsigned port)
            : m_ip(ip) 
            , m_port(port)
        {}

        inline const std::string& GetIP() const { return m_ip; }
        inline unsigned GetPort() const { return m_port; }
        
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ClientConnectRequestEvent: ip " << m_ip << " port " << m_port;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ClientConnectRequest)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetClient)

    private:
        std::string m_ip;
        unsigned m_port;
    };

    class SHINOBU_API ClientConnectResponseEvent : public Event
    {
    public:
        ClientConnectResponseEvent(bool isSuccessful)
            : m_successful(isSuccessful) {}

        inline const bool& IsSuccessful() const { return m_successful; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ClientConnectResponseEvent: successful " << m_successful;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ClientConnectResponse)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetClient)

    private:
        bool m_successful;
    };

    class SHINOBU_API ClientDisconnectRequestEvent : public Event
    {
    public:
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ClientDisconnectRequestEvent";
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ClientDisconnectRequest)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetClient)
    };

    class SHINOBU_API ClientDisconnectResponseEvent : public Event
    {
    public:
        ClientDisconnectResponseEvent(bool isGraceful)
            : m_graceful(isGraceful) {}

        inline const bool& IsGraceful() const { return m_graceful; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ClientDisconnectResponseEvent: graceful " << m_graceful;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ClientDisconnectResponse)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetClient)

    private:
        bool m_graceful;
    };

    class SHINOBU_API ClientSendPacketEvent : public Event
    {
    public:
        ClientSendPacketEvent(const Packet& packet)
            : m_packet(packet) {}

        inline Packet& GetPacket() { return m_packet; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ClientSendPacketEvent: packetID " << m_packet.id;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ClientSendPacket)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetClient)

    private:
        Packet m_packet;
    };

    class SHINOBU_API ClientReceivePacketEvent : public Event
    {
    public:
        ClientReceivePacketEvent(const Packet& packet)
            : m_packet(packet) {}

        inline Packet& GetPacket() { return m_packet; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ClientReceivePacketEvent: packetID " << m_packet.id;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ClientReceivePacket)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetClient)

    private:
        Packet m_packet;
    };

    ////////////////////////////////////////////
    // Server events
    ////////////////////////////////////////////

    class SHINOBU_API ServerHostRequestEvent : public Event
    {
    public:
        ServerHostRequestEvent(unsigned port)
            : m_port(port) {}

        unsigned GetPort() const { return m_port; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ServerHostRequestEvent: port " << m_port;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ServerHostRequest)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetServer)

    private:
        unsigned m_port;
    };

    class SHINOBU_API ServerHostResponseEvent : public Event
    {
    public:
        ServerHostResponseEvent(bool isSuccessful)
            : m_successful(isSuccessful) {}

        inline const bool& IsSuccessful() const { return m_successful; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ServerHostResponseEvent: successful " << m_successful;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ServerHostResponse)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetServer)

    private:
        bool m_successful;
    };

    class SHINOBU_API ServerClientConnectEvent : public Event
    {
    public:
        ServerClientConnectEvent(ENetPeer* peer)
            : m_peer(peer) {}

        inline ENetPeer* GetPeer() const { return m_peer; }

        std::string ToString() const override
        {
            std::stringstream ss;
            char ip[64];
            enet_address_get_host_ip(&m_peer->address, ip, 64);
            ss << "ServerClientConnectEvent: peer " << ip << ':' << m_peer->address.port;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ServerClientConnect)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetServer)

    private:
        ENetPeer* m_peer;
    };

    class SHINOBU_API ServerClientDisconnectEvent : public Event
    {
    public:
        ServerClientDisconnectEvent(ENetPeer* peer)
            : m_peer(peer) {}

        inline ENetPeer* GetPeer() const { return m_peer; }

        std::string ToString() const override
        {
            std::stringstream ss;
            char ip[64];
            enet_address_get_host_ip(&m_peer->address, ip, 64);
            ss << "ServerClientDisconnectEvent: peer " << ip << ':' << m_peer->address.port;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ServerClientDisconnect)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetServer)

    private:
        ENetPeer* m_peer;
    };

    class SHINOBU_API ServerShutdownRequestEvent : public Event
    {
    public:
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ServerShutdownRequestEvent";
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ServerShutdownRequest)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetServer)
    };

    class SHINOBU_API ServerShutdownResponseEvent : public Event
    {
    public:
        ServerShutdownResponseEvent(bool isGraceful)
            : m_graceful(isGraceful) {}

        inline const bool& IsGraceful() const { return m_graceful; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ServerShutdownResponseEvent: graceful " << m_graceful;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ServerShutdownResponse)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetServer)

    private:
        bool m_graceful;
    };

    class SHINOBU_API ServerSendPacketEvent : public Event
    {
    public:
        ServerSendPacketEvent(const Packet& packet, ENetPeer* peer)
            : m_packet(packet) 
            , m_peer(peer)
        {}

        inline Packet& GetPacket() { return m_packet; }
        inline ENetPeer* GetPeer() { return m_peer; }

        std::string ToString() const override
        {
            std::stringstream ss;
            char ip[64];
            enet_address_get_host_ip(&m_peer->address, ip, 64);
            ss << "ServerSendPacketEvent: to " << ip << ':' << m_peer->address.port << " packetID " << m_packet.id;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ServerSendPacket)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetServer)

    private:
        Packet m_packet;
        ENetPeer* m_peer;
    };

    class SHINOBU_API ServerBroadcastPacketEvent : public Event
    {
    public:
        ServerBroadcastPacketEvent(const Packet& packet)
            : m_packet(packet) {}

        inline Packet& GetPacket() { return m_packet; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ServerBroadcastPacketEvent: packetID " << m_packet.id;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ServerBroadcastPacket)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetServer)

    private:
        Packet m_packet;
    };

    class SHINOBU_API ServerReceivePacketEvent : public Event
    {
    public:
        ServerReceivePacketEvent(const Packet& packet)
            : m_packet(packet) {}

        inline Packet& GetPacket() { return m_packet; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "ServerReceivePacketEvent: packetID " << m_packet.id;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::ServerReceivePacket)
        EVENT_CLASS_CATEGORY(EventCategoryNet | EventCategoryNetServer)

    private:
        Packet m_packet;
    };
}