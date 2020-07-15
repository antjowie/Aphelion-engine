#pragma once
#include "Shinobu/Core/Core.h"
#include "Shinobu/Core/Layer.h"

namespace sh
{
    /**
     * Some thoughts to write down about the responsibilities
     *
     * Initially, the client layer would handle all the packets and stuff so that we can just listen to events. There are two
     * issues with this system.
     * 
     * 1. We can't submit events to the application without coupling our gameplay systems to the application. This is
     *    not such a bad issue, we can just add a PublishEvent method to the Application since the application is already
     *    globally accessible. 
     * 2. Modyfying the client (setting packet rate, getting info such as ping, loss rate, rtt) is not really possible
     *    since the client is encapsulated in the layer. 
     *
     * With these two issues, I thought I'd make the client and server static since I don't know of a situation where more
     * than one of these is required. But this brings the issue that the whole event system becomes redundant. 
     *
     * I decided to roll with it however. NetLayers will be responsible for handling packets on static objects. 
     * This way, we can easily still submit packets by just calling the client and server directly. 
     */

    class SHINOBU_API NetClientLayer : public Layer
    {
    public:
        NetClientLayer() : Layer("NetClientLayer") {}

        using EventCB = std::function<void(Event&)>;

        virtual void OnEvent(Event& event) override;
        virtual void OnUpdate(Timestep ts) override;
        virtual void OnDetach() override;
        void SetEventCB(const EventCB& cb);
        
    private:
        EventCB m_cb;
    };
}