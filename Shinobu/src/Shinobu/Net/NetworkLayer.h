#pragma once
#include <Shinobu/Common.h>
#include <Shinobu/Core/Window.h>

#include "Net/Client.h"
#include "Net/Server.h"

class ServerLayer : public sh::Layer
{
public:
    virtual void OnAttach() override
    {
        m_server.Host(25565);
    }
    
    virtual void OnUpdate(sh::Timestep ts) override final
    {
        ENetEvent event;
        /* Wait up to 1000 milliseconds for an event. */
        while (m_server.Poll(event) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                printf("A new client connected from %x:%u.\n",
                    event.peer->address.host,
                    event.peer->address.port);
                
                /* Store any relevant client information here. */
                    event.peer->data = "Client information";
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    event.peer->data,
                    event.channelID);
                
                /* Clean up the packet now that we're done using it. */
                    enet_packet_destroy(event.packet);

                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", event.peer->data);
                /* Reset the peer's client information. */
                event.peer->data = NULL;
            }
        }

        static unsigned count = 0;
        auto* pck = enet_packet_create(&(++count), sizeof(count), ENET_PACKET_FLAG_RELIABLE);

        m_server.Broadcast(pck);
        
        //enet_host_broadcast(m_host, 0, pck);
        //enet_host_flush(m_host);
    }
    
private:

    Server m_server;
};

class ClientLayer : public sh::Layer
{
public:
    virtual void OnAttach() override
    {
        m_client.Connect("localhost", 25565);
    }
    
    virtual void OnUpdate(sh::Timestep ts) override final
    {
        ENetEvent event;
        while (m_client.Poll(event) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet of length %u containing %i was received from %s on channel %u.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    event.peer->data,
                    event.channelID);
                break;
            }
        }

        unsigned code = 10;
        auto pck = enet_packet_create(&code, sizeof(code), ENET_PACKET_FLAG_RELIABLE);

        m_client.SendPacket(pck);
    }
    
private:

    Client m_client;
};