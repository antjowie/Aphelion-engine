#include "Server.h"

Server::Server()
    : m_host(nullptr)
    , m_connectCB(nullptr)
    , m_disconnectCB(nullptr)
{
}

Server::~Server()
{
    if (IsHosting()) Close();
}

bool Server::IsHosting() const
{
    return m_host;
}

bool Server::Host(unsigned port)
{
    ENetAddress address;
    enet_address_set_host(&address, "localhost");
    address.port = 25565;

    m_host = enet_host_create(&address, 32, 1, 0, 0);

    if (m_host == nullptr)
    {
        SH_ERROR("An error occurred while trying to create an ENet server host");
        return false;
    }

    SH_INFO("Hosting! Listening on {0}:{1}", address.host, port);
    return true;
}

void Server::Close()
{
    SH_ASSERT(IsHosting(), "Can't shutdown a server that is not hosting");

    // TODO: Gracefully disconnect clients
    SH_WARN("Server forcibly shut down!");
    enet_host_destroy(m_host);
}

bool Server::Poll(std::shared_ptr<Packet>& packet)
{
    ENetEvent event;

    int status =  enet_host_service(m_host,&event,0);
    SH_ASSERT(status >= 0, "Server returned with error");
    
    if (status == 0)
        return false;

    switch (event.type)
    {
    case ENET_EVENT_TYPE_CONNECT:
    {
        if(m_connectCB) m_connectCB(*this, event.peer->address);
        return Poll(packet);
    }
    break;
    case ENET_EVENT_TYPE_DISCONNECT:
        if (m_disconnectCB) m_disconnectCB(*this, event.peer->address);
        return Poll(packet);
        break;

    case ENET_EVENT_TYPE_RECEIVE:
        auto* data = event.packet->data;
        auto length = event.packet->dataLength;

        packet = std::shared_ptr<Packet>(PacketFromBinary(data, length).release());

        enet_packet_destroy(event.packet);
        return true;
        break;
    }
}