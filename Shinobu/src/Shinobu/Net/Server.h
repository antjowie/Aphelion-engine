#pragma once

#include "Shinobu/Core/Core.h"
#include "Shinobu/Net/Packet.h"

#include <enet/enet.h>
#include <functional>
#include <vector>

namespace sh
{
    class SHINOBU_API NetServer : public NonCopyable
    {
    public:
        // TODO: Maybe wrap ENetPeer
        using ConnectCB = std::function<void(NetServer&, ENetPeer* connection)>;

    public:
        static NetServer& Get();

        bool IsHosting() const;

        bool Host(unsigned port);
        void Shutdown();

        std::vector<ENetPeer*> GetConnections() const;

        void Broadcast(const Packet& packet);
        void Submit(const Packet& packet, ENetPeer* connection);
        void Flush();

        // Returns true if a packet has been received
        bool Poll(Packet& packet);

        void SetConnectCB(ConnectCB cb) { m_connectCB = cb; }
        void SetDisconnectCB(ConnectCB cb) { m_disconnectCB = cb; }

    private:
        NetServer();
        ~NetServer();

        ENetHost* m_socket;
    
        ConnectCB m_connectCB;
        ConnectCB m_disconnectCB;
    };
}

//template<typename T>
//inline void NetServer::Broadcast(const T& packet)
//{
//    auto stream = PacketToBinary(packet);
//    
//    //std::stringstream copy;
//    //copy << stream.str();
//    //
//    //auto val = PacketFromBinary(copy);
//
//    //SH_CORE_TRACE("Gave ID {0}", static_cast<JoinResponsePacket*>(val.get())->userID);
//
//    //stream.seekg(0, std::ios::end);
//    auto buffer = stream.str();
//    ENetPacket* pck = enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE);
//
//    enet_host_broadcast(m_socket, 0, pck);
//    enet_host_flush(m_socket);
//}