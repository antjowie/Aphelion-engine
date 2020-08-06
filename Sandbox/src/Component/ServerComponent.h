#pragma once
// TODO: Should be including the protocol header
#include <Aphelion/Net/Packet.h>
#include <Aphelion/ECS/Registry.h>

#define EMPTY_COMPONENT(Component) \
        inline bool operator==(const Component& lhs, const Component& rhs) { return true; } \
        template <typename S> void serialize(S& s, Component& o) {}

struct SenderComponent
{
    ENetPeer* peer;
    ap::Entity entity;
};
EMPTY_COMPONENT(SenderComponent);

inline void RegisterServerComponents()
{
    ap::Registry::RegisterComponent<SenderComponent>();
}