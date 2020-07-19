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
#include "entt/entity/fwd.hpp"
//#include "Shinobu/ECS/Registry.h"

//struct _ENetPacket;
//struct _ENetPeer;
#include <functional>

namespace sh
{
    struct Packet;
    //using SerializeFn = void(*)(Packet& packet);
    using SerializeFn = std::function<void(Packet& packet)>;

    // Defer the serialization
    template <typename T>
    class SHINOBU_API SerializeData
    {
    public:
        SerializeData(const T& data) : m_data(data) {}
        void operator()(Packet& packet) const;

    private:
        T m_data;
    };

    /**
     * A packet is a piece of data that is transmitted via the network
     * It is what the client and server send to each other
     */
    struct SHINOBU_API Packet
    {
        using Buffer = std::vector<uint8_t>;
        using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
        using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

        // Header data
        unsigned id = 0; // ID mostly refers to component ID but can also send custom ID
        unsigned entity = 0; // TODO: Coupled to ECS, should probably be changed
        unsigned clientSimulation = 0;
        unsigned serverSimulation = 0;

        // ENet data
        size_t size; // Set by ENet when received, used in deserializing
        _ENetPeer* sender = nullptr;
        Buffer buffer;

        SerializeFn serializeFn;
    };

    /**
     * Defines the header serialization code
     */
    template <typename T>
    SHINOBU_API void SerializeHeader(T& serializer, Packet& packet)
    {
        serializer.value4b(packet.id);
        serializer.value4b(packet.entity);
        serializer.value4b(packet.clientSimulation);
        serializer.value4b(packet.serverSimulation);
    }

    template<typename T>
    inline void SerializeData<T>::operator()(Packet& packet) const
    {
        bitsery::Serializer<Packet::OutputAdapter> ser{ packet.buffer };
        SerializeHeader(ser, packet);

        ser.object(m_data);
        ser.adapter().flush();
        packet.size = ser.adapter().writtenBytesCount();
    }

    template <typename T> 
    SHINOBU_API Packet Serialize(const T& data, entt::entity entity)
    {
        Packet packet;
        packet.id = entt::type_info<T>::id();
        packet.entity = entt::to_integral(entity);

        packet.serializeFn = SerializeData(data);
        return packet;
    }

    template <typename T> SHINOBU_API T Deserialize(Packet& packet)
    {
        T data;

        bitsery::Deserializer<Packet::InputAdapter> des{ packet.buffer.begin(), packet.size };
        SerializeHeader(des, packet);

        des.object(data);

        //same as serialization, but returns deserialization state as a pair
        //first = error code, second = if buffer was successfully read from begin to the end.
        auto state = std::make_pair(des.adapter().error(), des.adapter().isCompletedSuccessfully());
        SH_CORE_ASSERT(state.first == bitsery::ReaderError::NoError && state.second, "Packet deserializing failed");
        return data;
    }

    inline _ENetPacket* SHINOBU_API PackENetPacket(const Packet& packet)
    {
        //packet.serializeFn(packet);
        auto p = packet;
        p.serializeFn(p);
        return enet_packet_create(p.buffer.data(), p.size, ENET_PACKET_FLAG_UNSEQUENCED);
        //return enet_packet_create(packet.buffer.data(), packet.size, ENET_PACKET_FLAG_RELIABLE);
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
        SerializeHeader(des, p);

        enet_packet_destroy(packet);

        return p;
    }

}