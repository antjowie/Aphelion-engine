#pragma once
#include "Shinobu/Core/Core.h"

namespace sh
{
    // TODO: This will need to be moved to its own file if we are 
    // going to serialize others things as well
    struct SHINOBU_API BinaryBuffer
    {
    
    };
    
    /**
     * A packet is a piece of data that is transmitted via the network
     * It is what the client and server send to each other
     */
    struct SHINOBU_API Packet
    {
    
    };
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