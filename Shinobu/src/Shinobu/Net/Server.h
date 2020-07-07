#pragma once

#include "Net/Packet.h"

#include <enet/enet.h>
#include <functional>

class Server
{
public:
    // TODO: Change ENetAddress to an actual address instead of using enet
    using ConnectCB = std::function<void(Server&, ENetAddress user)>;

public:
    Server();
    ~Server();

    bool IsHosting() const;

    bool Host(unsigned port);
    void Close();

    template <typename T>
    void Broadcast(const T& packet);
    // Returns true if a packet has been received
    bool Poll(std::shared_ptr<Packet>& packet);

    void SetConnectCB(ConnectCB cb) { m_connectCB = cb; }
    void SetDisconnectCB(ConnectCB cb) { m_disconnectCB = cb; }

private:
    ENetHost* m_host;

    ConnectCB m_connectCB;
    ConnectCB m_disconnectCB;
};

template<typename T>
inline void Server::Broadcast(const T& packet)
{
    auto stream = PacketToBinary(packet);
    
    //std::stringstream copy;
    //copy << stream.str();
    //
    //auto val = PacketFromBinary(copy);

    //SH_CORE_TRACE("Gave ID {0}", static_cast<JoinResponsePacket*>(val.get())->userID);

    //stream.seekg(0, std::ios::end);
    auto buffer = stream.str();
    ENetPacket* pck = enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE);

    enet_host_broadcast(m_host, 0, pck);
    enet_host_flush(m_host);
}