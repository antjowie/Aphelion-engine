#pragma once
// TODO: Should be including the protocol header
#include <Shinobu/Net/Packet.h>
#include <Shinobu/ECS/Registry.h>

#define EMPTY_COMPONENT(Component) \
        inline bool operator==(const Component& lhs, const Component& rhs) { return true; } \
        template <typename S> void serialize(S& s, Component& o) {}

struct SenderComponent
{
    ENetPeer* peer;
    sh::Entity entity;
};
EMPTY_COMPONENT(SenderComponent);

inline void RegisterServerComponents()
{
    sh::Registry::RegisterComponent<SenderComponent>();
}