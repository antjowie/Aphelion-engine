#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Layer.h"
#include "Shinobu/Net/Protocol.h"

#include <enet/enet.h>

namespace sh
{
    class SHINOBU_API NetServerLayer : public Layer
    {
    public:
        using EventCB = std::function<void(Event&)>;

    public:
        NetServerLayer() : Layer("NetServerLayer") {}

        virtual void OnEvent(sh::Event& event) override;
        virtual void OnAttach() override;
        virtual void OnUpdate(sh::Timestep ts) override final;

        void SetEventCB(const EventCB& cb);

        static HostConfig m_config;

    private:
        /// Key = componentID Value = simulation
        using SimulationMap = std::unordered_map<unsigned, int>;

    private:
        EventCB m_cb;

        /// For every peer, we store their last packets and on which frame they were send 
        std::unordered_map<ENetPeer*, SimulationMap> m_peerData;
    };
}