#include "Shinobu/Net/Client.h"
#include "Shinobu/Core/Time.h"

namespace sh
{
    Client::Client()
        : m_socket(enet_host_create(nullptr, 1, 1, 0, 0))
        , m_server(nullptr)
    {
        if (m_socket == nullptr)
        {
            SH_CORE_ERROR("An error occurred while trying to create an ENet client host");
        }
    }

    Client::~Client()
    {
        if(IsConnected()) Disconnect();
        enet_host_destroy(m_socket);
    }


    bool Client::IsConnected() const
    {
        return m_socket->connectedPeers == 1;
    }

    bool Client::Connect(const std::string& host, unsigned port)
    {
        ENetAddress address;
        ENetEvent event;
        enet_address_set_host(&address, host.c_str());
        address.port = port;

        m_server = enet_host_connect(m_socket, &address, 1, 0);
        if (m_server == nullptr)
        {
            SH_CORE_ERROR("No available peers for initiating an ENet connection, increase host peer count");
            return false;
        }

        // TODO: Send an event upon successfull connection (basically thread this cuz it can hold for 5 seconds)
        /* Wait up to 5 seconds for the connection attempt to succeed. */
        SH_CORE_TRACE("Attempting connection with {0}:{1}", address.host, port);
        if (enet_host_service(m_socket, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
        {
            SH_CORE_INFO("Successfully connected with {0}:{1}", address.host, port);
        }
        else
        {
            /* Either the 5 seconds are up or a disconnect event was */
            /* received. Reset the peer in the event the 5 seconds   */
            /* had run out without any significant event.            */
            enet_peer_reset(m_server);
            SH_CORE_ERROR("Failed to connect with {0}:{1}", address.host, port);
            return false;
        }
        return true;
    }

    void Client::Disconnect()
    {
        SH_CORE_ASSERT(IsConnected(), "Client can't disconnect if they are not connected!");
        SH_CORE_TRACE("Attempting disconnect from server");

        // Attempt graceful disconnect
        enet_peer_disconnect(m_server, 0);

        sh::Time timer;

        while (timer.Total().Seconds() < 5.f)
        {
            ENetEvent event;
            while (enet_host_service(m_socket, &event, 0) > 0)
            {
                if (event.type == ENET_EVENT_TYPE_RECEIVE)
                    enet_packet_destroy(event.packet);
                else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
                {
                    SH_CORE_TRACE("Gracefully disconnected from server");
                    return;
                }
            }
        }
    
        SH_CORE_WARN("Forcefully disconnected form the server");
        enet_peer_reset(m_server);
    }

    void Client::Flush()
    {
        enet_host_flush(m_socket);
    }

    bool Client::Poll(Packet* packet)
    {
        ENetEvent event;

        int status = enet_host_service(m_socket, &event, 0);
        SH_CORE_ASSERT(status >= 0, "Client returned with error");

        if (status == 0)
            return false;

        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            SH_CORE_TRACE("Connected to {0}:{1}", event.peer->address.host, event.peer->address.port);
            // We do this since connecting and disconnecting don't actually return a packet
            return Poll(packet); 
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            SH_CORE_TRACE("Disconnected from {0}:{1}", event.peer->address.host, event.peer->address.port);
            return Poll(packet);
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            auto* data = event.packet->data;
            auto length = event.packet->dataLength;

            //packet = PacketFromBinary(data, length);

            enet_packet_destroy(event.packet);
            return true;
            break;
        }
    }
}
