#include "Aphelion/Net/ServerLayer.h"
#include "Aphelion/Net/Server.h"
#include "Aphelion/Event/NetEvent.h"

namespace ap
{
    HostConfig NetServerLayer::m_config;

    void NetServerLayer::OnEvent(ap::Event& event)
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
            AP_CORE_WARN("ServerLayer received {} but server isn't hosting",event.GetName());
            return;
        }

        if (d.Dispatch<ServerSendPacketEvent>([&](ServerSendPacketEvent& e)
            {
                auto& p = e.GetPacket();
                if (m_peerData[e.GetPeer()].count(p.id) == 0) m_peerData[e.GetPeer()][p.id] = -1;
                p.clientSimulation = m_peerData[e.GetPeer()][p.id];
                //AP_WARN(p.clientSimulation);
                server.Submit(p, e.GetPeer());
                return true;
            }));
        if (d.Dispatch<ServerBroadcastPacketEvent>([&](ServerBroadcastPacketEvent& e)
            {
                for (auto* peer : NetServer::Get().GetConnections())
                {
                    m_cb(ServerSendPacketEvent(e.GetPacket(), peer));
                }

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
                AP_CORE_INFO("Server opened connection with {}:{}", ip, connection->address.port);

                m_cb(ServerClientConnectEvent(connection));
            });
        server.SetDisconnectCB([&](NetServer& server, ENetPeer* connection)
            {
                char ip[64];
                enet_address_get_host_ip(&connection->address, ip, 64);
                AP_CORE_INFO("Server closed connection with {}:{}", ip, connection->address.port);
                m_peerData.erase(connection);

                m_cb(ServerClientDisconnectEvent(connection));
            });
    }

    void NetServerLayer::OnUpdate(ap::Timestep ts)
    {
        auto& server = NetServer::Get();
        if (!server.IsHosting()) return;

        static Timer timer;
        if (timer.Elapsed() > m_config.rate)
        {
            timer.Reset();
            Packet p;

            server.Flush();

            while (server.Poll(p))
            {
                // NOTE: Right here, we can easily add sequencing, but we do that with PacketBuffers. 
                // There is some unclearity who is responsible for what, so be sure to keep that in mind
                // and maybe refactor the system
                if (m_peerData[p.sender].count(p.id) == 0) m_peerData[p.sender][p.id] = -1;
                auto& clientSim = m_peerData[p.sender][p.id];
                if (clientSim < p.clientSimulation) // If we retrieve a newer sequence
                {
                    clientSim = p.clientSimulation;
                    m_cb(ServerReceivePacketEvent(p));
                }
            }
        }
    }

    void NetServerLayer::SetEventCB(const EventCB& cb)
    {
        m_cb = cb;
    }
}