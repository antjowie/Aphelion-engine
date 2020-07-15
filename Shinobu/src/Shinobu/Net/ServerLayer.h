#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Layer.h"

namespace sh
{
    class SHINOBU_API NetServerLayer : public Layer
    {
    public:
        NetServerLayer() : Layer("NetServerLayer") {}

        using EventCB = std::function<void(Event&)>;

        virtual void OnEvent(sh::Event& event) override;
        virtual void OnAttach() override;
        virtual void OnUpdate(sh::Timestep ts) override final;

        void SetEventCB(const EventCB& cb);

    private:
        EventCB m_cb;
    };
}