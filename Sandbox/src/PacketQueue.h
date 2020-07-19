#pragma once
#include <Shinobu/Net/Packet.h>
#include <map>

/**
 * The Queue makes sure that we only get the last verison (sequensing) of a packet
 * It also allows us to poll the queue whenever we want
 * 
 * Why is this needed ? so that we can control the order of things and not 
 * have desyncs where sometimes we use client predicted values and other times
 * we use packet values
 */
class PacketQueue
{
public:
    struct PacketKey
    {
        unsigned entity;
        unsigned id;

        bool operator<(const PacketKey& rhs) const
        {
            // https://stackoverflow.com/questions/1102392/how-can-i-use-stdmaps-with-user-defined-types-as-key
            // Apparently a key is equal if a is not smaller then b and likewise
            return (entity + id) < (rhs.entity + rhs.id);
        }
    };

public:
    inline void Push(sh::Packet& packet, bool isClient)
    {

        auto& map = GetMap();
        PacketKey key { packet.entity, packet.id };
        
        if (map.count(key) == 0)
            map[key] = packet;
        else
        {
            auto& val = map[key];
            unsigned oldSim = isClient ? val.clientSimulation : val.serverSimulation;
            unsigned newSim = isClient ? packet.clientSimulation : packet.serverSimulation;
            if (oldSim < newSim)
                val = packet;
            else if(oldSim != newSim)
            {
                SH_WARN("Packet came out of sequence. This should never happen since ENet sequences packets");
                SH_WARN("Packet type {} Old #{} Received #{}", packet.id, oldSim, newSim);
            }
        }
    }
    bool Poll(sh::Packet& packet)
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

        SH_ASSERT(GetMap().size() == 0, "Map should have been cleared by poll");
        //GetMap().clear();
    }

private:
    std::map<PacketKey, sh::Packet>& GetMap() { return m_packets[m_index]; }

    std::map<PacketKey, sh::Packet>& GetBackMap() { return m_packets[(m_index + 1) % 2]; }

    int m_index;
    std::map<PacketKey, sh::Packet> m_packets[2];
};