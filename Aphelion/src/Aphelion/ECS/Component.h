#pragma once
#include "Aphelion/Core/Transform.h"

#include <bitsery/traits/string.h>
#include <string>

//#define EMPTY_COMPONENT(Component)

namespace ap
{
    struct APHELION_API TransformComponent
    {
        Transform t;
    };
    template<typename S> void serialize(S& s, TransformComponent& v) { serialize(s, v.t); }
    inline bool operator==(const TransformComponent& lhs, const TransformComponent& rhs) { return lhs.t == rhs.t; }

    struct APHELION_API TagComponent
    {
        std::string tag;
    };
    template<typename S> void serialize(S& s, TagComponent& v) { s.text1b(v.tag,32); }
    inline bool operator==(const TagComponent& lhs, const TagComponent& rhs) { return lhs.tag == rhs.tag; }

    struct APHELION_API GUIDComponent
    {
        unsigned guid;
        operator unsigned() const {return guid;}
    };
    template<typename S> void serialize(S& s, GUIDComponent& v) { s.value4b(v.guid); }
    inline bool operator==(const GUIDComponent& lhs, const GUIDComponent& rhs) { return lhs.guid == rhs.guid; }
}