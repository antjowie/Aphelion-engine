#include "Client.h"

#include <Shinobu/Core/Timer.h>

Client::Client()
    : m_host(enet_host_create(nullptr, 1, 1, 0, 0))
    , m_client(nullptr)
{
    if (m_host == nullptr)
    {
        SH_ERROR("An error occurred while trying to create an ENet client host");
    }
}

Client::~Client()
{
    if(IsConnected()) Disconnect();
}


bool Client::IsConnected() const
{
    return m_host->connectedPeers == 1;
}

bool Client::Connect(const std::string& host, unsigned port)
{
    ENetAddress address;
    ENetEvent event;
    enet_address_set_host(&address, host.c_str());
    address.port = port;

    m_client = enet_host_connect(m_host, &address, 1, 0);
    if (m_client == nullptr)
    {
        SH_ERROR("No available peers for initiating an ENet connection");
        return false;
    }

    /* Wait up to 5 seconds for the connection attempt to succeed. */
    SH_TRACE("Attempting connection with {0}:{1}", address.host, port);
    if (enet_host_service(m_host, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        SH_INFO("Successfully connected with {0}:{1}", address.host, port);
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(m_client);
        SH_ERROR("Failed to connect with {0}:{1}", address.host, port);
        return false;
    }
    return true;
}

void Client::Disconnect()
{
    SH_ASSERT(IsConnected(), "Client can't disconnect if they are not connected!");

    // Attempt graceful disconnect
    enet_peer_disconnect(m_client, 0);

    sh::Timer timer;

    while (timer.Seconds() < 5.f)
    {
        ENetEvent event;
        while (enet_host_service(m_host, &event, 0) > 0)
        {
            if (event.type == ENET_EVENT_TYPE_RECEIVE)
                enet_packet_destroy(event.packet);
            else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
            {
                SH_TRACE("Gracefully disconnected from server");
                return;
            }
        }
    }
    
    SH_WARN("Forcefully disconnected form the server");
    enet_peer_reset(m_client);
}

bool Client::Poll(std::shared_ptr<Packet>& packet)
{
    ENetEvent event;

    int status = enet_host_service(m_host, &event, 0);
    SH_ASSERT(status >= 0, "Client returned with error");

    if (status == 0)
        return false;

    switch (event.type)
    {
    case ENET_EVENT_TYPE_CONNECT:
        SH_TRACE("Connected to {0}:{1}", event.peer->address.host, event.peer->address.port);
        // We do this since connecting and disconnecting don't actually return a packet
        return Poll(packet); 
        break;
    case ENET_EVENT_TYPE_DISCONNECT:
        SH_TRACE("Disconnected from {0}:{1}", event.peer->address.host, event.peer->address.port);
        return Poll(packet);
        break;
    case ENET_EVENT_TYPE_RECEIVE:
        auto* data = event.packet->data;
        auto length = event.packet->dataLength;

        packet = PacketFromBinary(data, length);

        enet_packet_destroy(event.packet);
        return true;
        break;
    }
}
