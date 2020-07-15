#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Time.h"
#include "Shinobu/Net/Packet.h"

#include <enet/enet.h>
#include <functional>
#include <string>
#include <future>

namespace sh
{
    class SHINOBU_API NetClient : public NonCopyable
    {
    public:
        NetClient(const NetClient&) = delete;

        static NetClient& Get();
    
        bool IsConnected() const;
        /** 
         * Returns true only while connecting
         */
        bool IsConnecting() const;

        std::shared_future<bool> Connect(const std::string& host, unsigned port, const Timestep& timeout = Timestep(5));
        std::shared_future<bool> Disconnect(const Timestep& timeout = Timestep(5));
    
        void Submit(const Packet& packet);
        void Flush();
    
        bool Poll(Packet& packet);
    
    private:
        NetClient();
        ~NetClient();

        bool m_isConnecting;
        std::shared_future<bool> m_connectFuture;

        ENetHost* m_socket;
        ENetPeer* m_server;
    };
}

//template<typename T>
//inline void NetClient::SendPacket(const T& packet)
//{
//    // Serialze the archive to binary
//    //auto stream = PacketToBinary(packet);
//    auto stream = PacketToBinary(static_cast<Packet*>(packet.get()));
//
//    stream.seekg(0, std::ios::end);
//    ENetPacket* pck = enet_packet_create(stream.rdbuf(), stream.tellg(), ENET_PACKET_FLAG_RELIABLE);
//
//    enet_peer_send(m_server, 0, pck);
//    enet_host_flush(m_socket);
//}