#pragma once

#include "Net/Packet.h"

#include <enet/enet.h>
#include <functional>
#include <string>

class Client
{
public:
    Client();
    ~Client();

    bool IsConnected() const;

    bool Connect(const std::string& host, unsigned port);
    void Disconnect();

    template <typename T>
    void SendPacket(const T& packet);
    bool Poll(std::shared_ptr<Packet>& packet);

private:
    ENetHost* m_socket;
    ENetPeer* m_server;
};

template<typename T>
inline void Client::SendPacket(const T& packet)
{
    // Serialze the archive to binary
    //auto stream = PacketToBinary(packet);
    auto stream = PacketToBinary(static_cast<Packet*>(packet.get()));

    stream.seekg(0, std::ios::end);
    ENetPacket* pck = enet_packet_create(stream.rdbuf(), stream.tellg(), ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(m_server, 0, pck);
    enet_host_flush(m_socket);
}