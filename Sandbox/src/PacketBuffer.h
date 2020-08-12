#pragma once
#include "Aphelion/Net/Packet.h"
#include <map>

/**
 * The Buffer makes sure that we only get the last version (sequencing) of a packet
 *
 * It also allows us to poll the queue whenever we want
 * Why is this needed ? so that we can control the order of things and not 
 * have desyncs where sometimes we use client predicted values and other times
 * we use packet values
 */
class PacketBuffer
{
public:
    inline void Push(ap::Packet& packet, bool isClient)
    {
        auto& map = GetMap();
        PacketKey key(packet);

        if (map.count(key) == 0)
            map[key] = packet;
        else
        {
            auto& val = map[key];
            //unsigned oldSim = isClient ? val.clientSimulation : val.serverSimulation;
            //unsigned newSim = isClient ? packet.clientSimulation : packet.serverSimulation;
            int oldSim = val.clientSimulation;
            int newSim = packet.clientSimulation;
            if (newSim > oldSim)
                val = packet;
            else if(oldSim != newSim)
            {
                AP_WARN("Packet came out of sequence. This should never happen since ENet sequences packets");
                AP_WARN("Packet type {} Old #{} Received #{}", packet.id, oldSim, newSim);
            }
        }
    }

    bool Poll(ap::Packet& packet)
    {
        auto& map = GetBackMap();
        auto begin = map.begin();
        if (begin == map.end())
            return false;
        packet = begin->second;
        map.erase(begin);
        return true;
    }

    void Swap()
    {
        m_index++;
        m_index %= 2;

        AP_ASSERT(GetMap().size() == 0, "Map should have been cleared by poll");
        //GetMap().clear();
    }

private:
    struct PacketKey
    {
        PacketKey() = default;
        PacketKey(const ap::Packet& packet)
            : guid(packet.guid)
            , id(packet.id)
        {}

        unsigned guid;
        unsigned id;

        bool operator<(const PacketKey& rhs) const
        {
            // https://stackoverflow.com/questions/1102392/how-can-i-use-stdmaps-with-user-defined-types-as-key
            // Apparently a key is equal if a is not smaller then b and likewise
            return (guid + id) < (rhs.guid + rhs.id);
        }
    };

    using Buffer = std::map<PacketKey, ap::Packet>;

private:
    Buffer& GetMap() { return m_packets[m_index]; }
    Buffer& GetBackMap() { return m_packets[(m_index + 1) % 2]; }

    int m_index;
    Buffer m_packets[2];
};