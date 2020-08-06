#include "Aphelion/Net/Server.h"

namespace ap
{
    NetServer::NetServer()
        : m_socket(nullptr)
        , m_connectCB(nullptr)
        , m_disconnectCB(nullptr)
    {
    }

    NetServer::~NetServer()
    {
        if (IsHosting()) Shutdown();
    }

    NetServer& NetServer::Get()
    {
        static NetServer instance;
        return instance;
    }

    bool NetServer::IsHosting() const
    {
        return m_socket;
    }

    bool NetServer::Host(unsigned port)
    {
        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = 25565;

        m_socket = enet_host_create(&address, 32, 1, 0, 0);

        if (m_socket == nullptr)
        {
            AP_CORE_ERROR("An error occurred while trying to create an ENet server host");
            return false;
        }

        char hostIP[128];
        enet_address_get_host_ip(&address, hostIP, 128);

        AP_CORE_INFO("Hosting! Listening on {0}:{1}", hostIP, port);
        return true;
    }

    void NetServer::Shutdown()
    {
        AP_CORE_ASSERT(IsHosting(), "Can't shutdown a server that is not hosting");

        // TODO: Gracefully disconnect clients
        AP_CORE_WARN("Server forcibly shut down!");
        enet_host_destroy(m_socket);
        m_socket = nullptr;
    }

    std::vector<ENetPeer*> NetServer::GetConnections() const
    {
        std::vector<ENetPeer*> connections;
        for (int i = 0; i < m_socket->connectedPeers; i++)
        {
            connections.push_back(&m_socket->peers[i]);
        }
        return connections;
    }

    void NetServer::Broadcast(Packet& packet)
    {
        //PreSendPacket(packet);
        //enet_host_broadcast(m_socket, 0, PackENetPacket(packet));
    
        for(auto* peer : GetConnections())
        {
            Submit(packet, peer);
        }
    }

    void NetServer::Submit(Packet& packet, ENetPeer* connection)
    {
        enet_peer_send(connection, 0, PackENetPacket(packet));
    }

    void NetServer::Flush()
    {
        enet_host_flush(m_socket);
    }

    bool NetServer::Poll(Packet& packet)
    {
        ENetEvent event;

        int status = enet_host_service(m_socket,&event,0);
        AP_CORE_ASSERT(status >= 0, "Server returned with error");
    
        if (status == 0)
            return false;

        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            if(m_connectCB) m_connectCB(*this, event.peer);
            return Poll(packet);
        }
        break;
        case ENET_EVENT_TYPE_DISCONNECT:
            if (m_disconnectCB) m_disconnectCB(*this, event.peer);
            return Poll(packet);
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            packet = UnpackENetPacket(event.packet);
            packet.sender = event.peer;

            //AP_CORE_TRACE("Server received a packet of size {}", packet.size);
            return true;
            break;
        }
    }
}