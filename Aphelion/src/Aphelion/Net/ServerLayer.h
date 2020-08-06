#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Layer.h"
#include "Aphelion/Net/Protocol.h"

#include <enet/enet.h>

namespace ap
{
    class APHELION_API NetServerLayer : public Layer
    {
    public:
        using EventCB = std::function<void(Event&)>;

    public:
        NetServerLayer() : Layer("NetServerLayer") {}

        virtual void OnEvent(ap::Event& event) override;
        virtual void OnAttach() override;
        virtual void OnUpdate(ap::Timestep ts) override final;

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