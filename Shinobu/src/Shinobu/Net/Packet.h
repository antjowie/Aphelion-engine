#pragma once
#include "Shinobu/Core/Core.h"

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/string.h>

struct _ENetPacket;
struct _ENetPeer;

namespace sh
{
    struct ExampleData
    {
        std::string message;
    };

    //define how object should be serialized/deserialized
    template <typename S>
    void serialize(S& s, ExampleData& o)
    {
        s.text1b(o.message, 128);
    }

    /**
     * A packet is a piece of data that is transmitted via the network
     * It is what the client and server send to each other
     */
    struct SHINOBU_API Packet
    {
        //some helper types
        using Buffer = std::vector<uint8_t>;
        using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
        using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

        size_t size; // Set by ENet when received, used in deserializing
        Buffer buffer;

        // Data
        _ENetPeer* sender = nullptr;
    };

    template <typename T> SHINOBU_API Packet Serialize(const T& data)
    {
        Packet packet;
        packet.size = bitsery::quickSerialization<Packet::OutputAdapter>(packet.buffer, data);
        return packet;
    }

    template <typename T> SHINOBU_API T Deserialize(Packet& packet)
    {
        T data;
        auto state = bitsery::quickDeserialization<Packet::InputAdapter>({ packet.buffer.begin(),packet.size }, data);

        //same as serialization, but returns deserialization state as a pair
        //first = error code, second = if buffer was successfully read from begin to the end.
        SH_CORE_ASSERT(state.first == bitsery::ReaderError::NoError && state.second, "Packet deserializing failed");
        return data;
    }

    _ENetPacket* SHINOBU_API MakeENetPacket(const Packet& packet);
}