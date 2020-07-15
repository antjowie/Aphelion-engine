#include "Shinobu/Net/ClientLayer.h"
#include "Shinobu/Net/Client.h"
#include "Shinobu/Event/NetEvent.h"

namespace sh
{
    static std::shared_future<bool> connectFuture;
    static std::shared_future<bool> disconnectFuture;

    inline bool HasValue(std::shared_future<bool>& future)
    {
        return future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    void NetClientLayer::OnDetach()
    {
        if(connectFuture.valid()) connectFuture.wait();
        if(disconnectFuture.valid()) disconnectFuture.wait();
    }

    void NetClientLayer::OnEvent(Event& event)
    {
        if (!event.IsInCategory(EventCategoryNetClient)) return;

        EventDispatcher d(event);
        auto& client = NetClient::Get();
        // Handle connection events
        if (d.Dispatch<ClientConnectRequestEvent>([&](ClientConnectRequestEvent& e)
            {
                connectFuture = client.Connect(e.GetIP(), e.GetPort());
                return false;
            })) return;

        if (d.Dispatch<ClientDisconnectRequestEvent>([&](ClientDisconnectRequestEvent& e)
            {
                disconnectFuture = client.Disconnect();
                return false;
            })) return;

        // Packet events
        if (!client.IsConnected() && event.GetEventType() != EventType::ClientDisconnectResponse)
        {
            SH_CORE_WARN("NetClientLayer received {} but client isn't connected", event.GetName());
            return;
        }

        if (d.Dispatch<ClientSendPacketEvent>([](ClientSendPacketEvent & e)
            {
                NetClient::Get().Submit(e.GetPacket());
                return true;
            })) return;
    }

    void NetClientLayer::OnUpdate(Timestep ts)
    {
        // If we attempt connection
        if (HasValue(connectFuture))
        {
            m_cb(ClientConnectResponseEvent(connectFuture.get()));
            connectFuture = std::shared_future<bool>(); // This resets the future
        }
        if (HasValue(disconnectFuture))
        {
            m_cb(ClientDisconnectResponseEvent(disconnectFuture.get()));
            disconnectFuture = std::shared_future<bool>(); // This resets the future
        }

        auto& client = NetClient::Get();
        if (!client.IsConnected()) return;

        // Handle packets
        Packet p;
        while (client.Poll(p))
        {
            m_cb(ClientReceivePacketEvent(p));
        }
        client.Flush();
    }

    void NetClientLayer::SetEventCB(const EventCB& cb)
    {
        m_cb = cb;
    }

}