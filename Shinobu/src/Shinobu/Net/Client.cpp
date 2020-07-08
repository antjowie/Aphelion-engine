#include "Shinobu/Net/Client.h"
#include "Shinobu/Core/Time.h"

namespace sh
{
    static std::atomic_bool cancelConnecting = false;

    Client::Client()
        : m_isConnecting(false)
        , m_socket(enet_host_create(nullptr, 1, 1, 0, 0))
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
        cancelConnecting = true;
        if(m_connectFuture.valid()) m_connectFuture.wait();

        enet_host_destroy(m_socket);
    }

    bool Client::IsConnected() const
    {
        return m_socket->connectedPeers == 1;
    }

    bool Client::IsConnecting() const
    {
        return m_isConnecting;
    }

    std::shared_future<bool> Client::Connect(const std::string& host, unsigned port, const Timestep& timeout)
    {
        if (m_isConnecting)
        {
            SH_CORE_WARN("The client is already attempting to connect! Cancel this by calling Disconnect");
            return m_connectFuture;
        }

        m_connectFuture = std::async(std::launch::async, [&, host, port, timeout]
            {
                m_isConnecting = true;

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
                
                SH_CORE_TRACE("Attempting connection with {0}:{1}", address.host, port);
                sh::Time timer;
                while (timer.Total().Seconds() < timeout.MilliSeconds() && !cancelConnecting)
                {
                    while(enet_host_service(m_socket, &event, 0) > 0)
                        switch (event.type)
                        {
                        case ENET_EVENT_TYPE_CONNECT:
                        {
                            SH_CORE_TRACE("Established connection with {}:{}", address.host, port);
                            m_isConnecting = false;
                            return true;
                        }
                        default:
                            SH_CORE_WARN("Received arbitrary type {} from {}:{}", event.type, address.host, port);
                        }
                }
                // Timed out or connection cancelled
                enet_peer_reset(m_server);
                if (cancelConnecting)
                {
                    SH_CORE_TRACE("Aborting connection attempt successful");
                    cancelConnecting = false;
                }
                else
                {
                    SH_CORE_WARN("Server did not respond in time out period ({} seconds)", timeout);
                }
                m_isConnecting = false;
                return false;
            });

        return m_connectFuture;
    }

    std::shared_future<void> Client::Disconnect(const Timestep& timeout)
    {
        static std::shared_future<void> future;

        if (m_isConnecting)
        {
            SH_CORE_TRACE("Aborting connection attempt...");
            cancelConnecting = true;
            return future;
        }

        future = std::async(std::launch::async, [&, timeout]
            {
                SH_CORE_ASSERT(IsConnected(), "Client can't disconnect if they are not connected!");
                SH_CORE_TRACE("Attempting disconnect from server...");

                // Attempt graceful disconnect
                enet_peer_disconnect(m_server, 0);
                sh::Time timer;

                while (timer.Total().Seconds() < timeout.MilliSeconds())
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
                return;
            });
        return future;
    }

    void Client::Submit(const Packet& packet)
    {
        enet_peer_send(m_server,0,sh::MakeENetPacket(packet));
    }

    void Client::Flush()
    {
        enet_host_flush(m_socket);
    }

    bool Client::Poll(Packet& packet)
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
            packet.size = event.packet->dataLength;
            uint8_t* data = event.packet->data;
            packet.buffer = sh::Packet::Buffer(data, data + packet.size);
            packet.sender = event.peer; // Should always be server

            enet_packet_destroy(event.packet);
            return true;
            break;
        }
    }
}
