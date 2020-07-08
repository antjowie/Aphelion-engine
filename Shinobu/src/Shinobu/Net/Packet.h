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
        s.text1b(o.message,128);
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
    
#if 0
    int main() {
        //set some random data
        MyStruct data{ 8941, MyEnum::V2, 0.045 };
        MyStruct res{};

        //open file stream for writing and reading
        auto fileName = "test_file.bin";
        std::fstream s{ fileName, s.binary | s.trunc | s.out };
        if (!s.is_open()) {
            std::cout << "cannot open " << fileName << " for writing\n";
            return 0;
        }

        //we cannot use quick serialization function, because streams cannot use writtenBytesCount method
        bitsery::Serializer<bitsery::OutputBufferedStreamAdapter> ser{ s };
        ser.object(data);
        //flush to writer
        ser.adapter().flush();
        s.close();
        //reopen for reading

        s.open(fileName, s.binary | s.in);
        if (!s.is_open()) {
            std::cout << "cannot open " << fileName << " for reading\n";
            return 0;
}

        //same as serialization, but returns deserialization state as a pair
        //first = error code, second = is buffer was successfully read from begin to the end.
        auto state = bitsery::quickDeserialization<bitsery::InputStreamAdapter>(s, res);

        assert(state.first == bitsery::ReaderError::NoError && state.second);
        assert(data.f == res.f && data.i == res.i && data.e == res.e);
}


    //include bitsery.h to get serialization and deserialization classes
    #include <bitsery/bitsery.h>
    //in ordered to serialize/deserialize data to buffer, include buffer adapter
    #include <bitsery/adapter/buffer.h>
    //bitsery itself doesn't is lightweight, and doesnt include any unnessessary files,
    //traits helps library to know how to use types correctly,
    //in this case we'll be using vector both, to serialize/deserialize data and to store use as a buffer.
    #include <bitsery/traits/vector.h>

    enum class MyEnum :uint16_t { V1, V2, V3 };
    struct MyStruct {
        uint32_t i;
        MyEnum e;
        std::vector<float> fs;
    };

    //define how object should be serialized/deserialized
    template <typename S>
    void serialize(S& s, MyStruct& o) {
        s.value4b(o.i);//fundamental types (ints, floats, enums) of size 4b
        s.value2b(o.e);
        s.container4b(o.fs, 10);//resizable containers also requires maxSize, to make it safe from buffer-overflow attacks
    }
#endif
}

// Legacy code
#if 0
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/base_class.hpp>

#include <sstream>
#include <memory>

enum class PacketType
{
    JoinResponse,
    EntityCommand
};

struct Packet
{
public:
    virtual PacketType GetType() const = 0;

    unsigned packetID = 1;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar(packetID);
    }
};

// Must be a smart pointer
template <typename T>
std::stringstream PacketToBinary(const T& packet)
{
    static_assert(
        std::is_base_of<std::unique_ptr<Packet>::element_type,
        T::element_type>::value, "T has to be derived from Packet");
    std::stringstream stream;
    {
        cereal::PortableBinaryOutputArchive out(stream);
        
        // NOTE: Cereal needs a pointer of base type
        // since it is too much of a hassle imo to convert all pointers to that type
        // I just convert it here. There may be some overhead by doing that
        std::unique_ptr<Packet> data(new T::element_type(*packet));
        out(data);
    }

    return stream;
}
std::unique_ptr<Packet> PacketFromBinary(std::stringstream& binary);
std::unique_ptr<Packet> PacketFromBinary(unsigned char* binary, unsigned count);
#endif