#pragma once
#include "Shinobu/Core/Core.h"

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/string.h>

#include "enet/enet.h"

// Net code is coupled to ECS
// This is bad and should not be a thing
#include "entt/core/type_info.hpp"

//struct _ENetPacket;
//struct _ENetPeer;

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
        using Buffer = std::vector<uint8_t>;
        using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
        using InputAdapter = bitsery::InputBufferAdapter<Buffer>;
        // Bitsery needs a class to serialize
        struct ID 
        {
            ID& operator= (unsigned rhs) { value = rhs; return *this; }
            operator unsigned int() const { return value; }
            unsigned value; 
        }; 

        ID id;
        ID entity; // TODO: Coupled to ECS, should probably be changed
        size_t size; // Set by ENet when received, used in deserializing
        _ENetPeer* sender = nullptr;

        Buffer buffer;
    };

    template <typename S>
    void serialize(S& s, Packet::ID& o)
    {
        s.value4b(o.value);
    }

    template <typename T> SHINOBU_API Packet Serialize(const T& data, unsigned id = 0)
    {
        Packet packet;
        //packet.size = bitsery::quickSerialization<Packet::OutputAdapter>(packet.meta.dataBuffer, data);
        //size_t quickSerialization(OutputAdapter adapter, const T & value) {
        //    Serializer<OutputAdapter> ser{ std::move(adapter) };

        //packet.id = 10;
        // TODO: Uncouple from net code
        // Coupled to net code
        packet.id = entt::type_info<T>::id();
        packet.entity = id;

        bitsery::Serializer<Packet::OutputAdapter> ser{ packet.buffer };
        ser.object(packet.id);
        ser.object(packet.entity);

        ser.object(data);
        ser.adapter().flush();
        packet.size = ser.adapter().writtenBytesCount();

        return packet;
    }

    template <typename T> SHINOBU_API T Deserialize(Packet& packet)
    {
        T data;

        //auto state = bitsery::quickDeserialization<Packet::InputAdapter>({ packet.buffer.begin(),packet.size }, data);
        //std::pair<ReaderError, bool> quickDeserialization(InputAdapter adapter, T & value) {
        //    Deserializer<InputAdapter> des{ std::move(adapter) };

        bitsery::Deserializer<Packet::InputAdapter> des{ packet.buffer.begin(), packet.size };
        des.object(packet.id);
        des.object(packet.entity);

        des.object(data);

        //same as serialization, but returns deserialization state as a pair
        //first = error code, second = if buffer was successfully read from begin to the end.
        auto state = std::make_pair(des.adapter().error(), des.adapter().isCompletedSuccessfully());
        SH_CORE_ASSERT(state.first == bitsery::ReaderError::NoError && state.second, "Packet deserializing failed");
        return data;
    }

    inline _ENetPacket* SHINOBU_API PackENetPacket(const Packet& packet)
    {
        return enet_packet_create(packet.buffer.data(), packet.size, ENET_PACKET_FLAG_UNSEQUENCED);
    }

    /** 
     * This will destroy the packet that is passed
     */
    inline Packet SHINOBU_API UnpackENetPacket(_ENetPacket* packet)
    {
        Packet p;
        p.size = packet->dataLength;
        uint8_t* data = packet->data;
        p.buffer = sh::Packet::Buffer(data, data + p.size);

        bitsery::Deserializer<Packet::InputAdapter> des{ p.buffer.begin(), p.size };
        des.object(p.id);
        des.object(p.entity);

        enet_packet_destroy(packet);

        return p;
    }
}