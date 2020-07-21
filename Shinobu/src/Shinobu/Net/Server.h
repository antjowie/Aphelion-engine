#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Net/Packet.h"

#include <enet/enet.h>

#include <functional>
#include <vector>
#include <map>

namespace sh
{
    /**
     * @brief The NetServer is responsible for keeping "connections" and handling packets
     * It's basically a host with multiple peers
     *
     * @details Features
     * Reconciliation:
     *  We want to store the simulation of last retrieved packet for each client. 
     *  This information is added when we send the packet so that the client knows how much
     *  it is ahead of the server
     *
     * NOTE: Right now the server suffers from multiple responsibilities.
     *       Initially, it was only supposed to retrieve and send packets.
     *       Right now, it also sequences packets. This also happens in PacketBuffer.
     *       This may have to be refactored
     */
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

        void Broadcast(Packet& packet);
        void Submit(Packet& packet, ENetPeer* connection);
        void Flush();

        /// Returns true if a packet has been received
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