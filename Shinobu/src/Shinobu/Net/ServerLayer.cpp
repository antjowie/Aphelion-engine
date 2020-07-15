#include "Shinobu/Net/ServerLayer.h"
#include "Shinobu/Net/Server.h"
#include "Shinobu/Event/NetEvent.h"

namespace sh
{
    void NetServerLayer::OnEvent(sh::Event& event)
    {
        if (!event.IsInCategory(EventCategoryNetServer)) return;
        EventDispatcher d(event);
        auto& server = NetServer::Get();

        if (d.Dispatch<ServerHostRequestEvent>([&](ServerHostRequestEvent& e)
            {
                m_cb(ServerHostResponseEvent(server.Host(e.GetPort())));
                return false;
            })) return;

        if (d.Dispatch<ServerShutdownRequestEvent>([&](ServerShutdownRequestEvent& e)
            {
                server.Shutdown();
                m_cb(ServerShutdownResponseEvent(true));
                return false;
            })) return;


        if (!server.IsHosting() && event.GetEventType() != EventType::ServerShutdownResponse)
        {
            SH_CORE_WARN("ServerLayer received {} but server isn't hosting",event.GetName());
            return;
        }

        if (d.Dispatch<ServerSendPacketEvent>([&](ServerSendPacketEvent& e)
            {
                server.Submit(e.GetPacket(), e.GetPeer());
                return true;
            }));
        if (d.Dispatch<ServerBroadcastPacketEvent>([&](ServerBroadcastPacketEvent& e)
            {
                server.Broadcast(e.GetPacket());
                return true;
            }));
    }

    void NetServerLayer::OnAttach() 
    {
        auto& server = NetServer::Get();

        // using ConnectCB = std::function<void(Server&, ENetPeer* connection)>;
        server.SetConnectCB([&](NetServer& server, ENetPeer* connection)
            {
                char ip[64];
                enet_address_get_host_ip(&connection->address, ip, 64);
                SH_CORE_INFO("Server opened connection with {}:{}", ip, connection->address.port);

                m_cb(ServerClientConnectEvent(connection));
            });
        server.SetDisconnectCB([&](NetServer& server, ENetPeer* connection)
            {
                char ip[64];
                enet_address_get_host_ip(&connection->address, ip, 64);
                SH_CORE_INFO("Server closed connection with {}:{}", ip, connection->address.port);

                m_cb(ServerClientDisconnectEvent(connection));
            });
    }

    void NetServerLayer::OnUpdate(sh::Timestep ts)
    {
        auto& server = NetServer::Get();
        if (!server.IsHosting()) return;

        sh::Packet p;
        while (server.Poll(p))
        {
            m_cb(ServerReceivePacketEvent(p));
        }

        server.Flush();
    }
    void NetServerLayer::SetEventCB(const EventCB& cb)
    {
        m_cb = cb;
    }
}