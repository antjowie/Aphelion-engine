#include "Shinobu/Net/Packet.h"

#include "enet/enet.h"

namespace sh
{
    _ENetPacket* MakeENetPacket(const Packet& packet)
    {
        return enet_packet_create(packet.buffer.data(), packet.size, ENET_PACKET_FLAG_UNSEQUENCED);
    }
}