#pragma once
#include <Shinobu/Renderer/Texture.h>

#include <string>

#include "Shinobu/Core/Transform.h"

// This si an invalid component type since it isnt' move constructibel tho I thought it could be used this way
//using Player = entt::tag<"Player"_hs>();

struct Player 
{
    char empty;
    bool operator==(const Player& rhs) const { return true; }
};
template <typename S> void serialize(S& s, Player& o) {}

struct Tag
{
    std::string tag;
    bool operator==(const Tag& rhs) const { return tag == rhs.tag; }
};

template <typename S> void serialize(S& s, Tag& o)
{
    s.text1b(o.tag,32);
}

struct Health
{
    int health;
    bool operator==(const Health& rhs) const {return health == rhs.health; }
};

template <typename S> void serialize(S& s, Health& o)
{
    s.value4b(o.health);
}

struct Sprite
{
    void LoadTexture() { tex = sh::Texture2D::Create(image.c_str()); }

    std::string image;

    // This should be a Texture2D object but not sure if there is a callback after serialize
    sh::TextureRef tex;

    bool operator==(const Sprite& rhs) const { return image == rhs.image; }
};

template <typename S>
void serialize(S& s, Sprite& o)
{
    constexpr int pathSize = 128;
    SH_ASSERT(o.image.size() < pathSize, "Image path is too long");
    s.text1b(o.image, pathSize);

    if (!o.tex) { o.LoadTexture(); }
}

inline void RegisterComponents()
{
    sh::Registry::RegisterComponent<Player>();
    sh::Registry::RegisterComponent<sh::Transform>();
    sh::Registry::RegisterComponent<Sprite>();
    sh::Registry::RegisterComponent<Health>();
}