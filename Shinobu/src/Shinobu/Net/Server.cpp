#include "Shinobu/Net/Server.h"

namespace sh
{
    Server::Server()
        : m_socket(nullptr)
        , m_connectCB(nullptr)
        , m_disconnectCB(nullptr)
    {
    }

    Server::~Server()
    {
        if (IsHosting()) Shutdown();
    }

    bool Server::IsHosting() const
    {
        return m_socket;
    }

    bool Server::Host(unsigned port)
    {
        ENetAddress address;
        enet_address_set_host(&address, "localhost");
        address.port = 25565;

        m_socket = enet_host_create(&address, 32, 1, 0, 0);

        if (m_socket == nullptr)
        {
            SH_CORE_ERROR("An error occurred while trying to create an ENet server host");
            return false;
        }

        SH_CORE_INFO("Hosting! Listening on {0}:{1}", address.host, port);
        return true;
    }

    void Server::Shutdown()
    {
        SH_CORE_ASSERT(IsHosting(), "Can't shutdown a server that is not hosting");

        // TODO: Gracefully disconnect clients
        SH_CORE_WARN("Server forcibly shut down!");
        enet_host_destroy(m_socket);
    }

    std::vector<ENetPeer*> Server::GetConnections() const
    {
        std::vector<ENetPeer*> connections;
        for (int i = 0; i < m_socket->peerCount; i++)
        {
            connections.push_back(&m_socket->peers[i]);
        }
        return connections;
    }

    void Server::Flush()
    {
        enet_host_flush(m_socket);
    }

    bool Server::Poll(Packet* packet)
    {
        ENetEvent event;

        int status =  enet_host_service(m_socket,&event,0);
        SH_CORE_ASSERT(status >= 0, "Server returned with error");
    
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
            auto* data = event.packet->data;
            auto length = event.packet->dataLength;

            //packet = std::shared_ptr<Packet>(PacketFromBinary(data, length).release());

            enet_packet_destroy(event.packet);
            return true;
            break;
        }
    }
}